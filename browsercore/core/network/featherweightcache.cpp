/****************************************************************************
**
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This file is part of Qt Web Runtime.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

# include <QAbstractNetworkCache>
# include <QNetworkCacheMetaData>
# include <QDateTime>
# include <QDir>
# include <QDirIterator>
# include <QFile>
# include <QtGlobal>
# include <QDebug>
# include <QQueue>
# include <featherweightcache.h>
# include <featherweightcache_p.h>
#if defined(Q_OS_SYMBIAN)
#include <e32std.h>
#endif

//#define FEATHERWEIGHTCACHE_DEBUG

#define CACHE_POSTFIX QLatin1String(".d")
#define PREPARED_SLASH QLatin1String("prepared/")
#define DATA_SLASH QLatin1String("data/")
#define MAX_COMPRESSION_SIZE (1024 * 1024 * 3)

namespace WRT {

/*!
    \class FeatherWeightCache

    \brief The FeatherWeightCache class provides a very basic disk cache.

    FeatherWeightCache stores each url in its own file inside of the
    cacheDirectory using QDataStream.  Files with a text MimeType
    are compressed using qCompress.  Each cache file starts with "cache_"
    and ends in ".cache".  Data is written to disk only in insert()
    and updateMetaData().

    Currently you can not share the same cache files with more then
    one disk cache.

    FeatherWeightCache by default limits the amount of space that the cache will
    use on the system to 50MB.

    Note you have to set the cache directory before it will work.

    A network disk cache can be enabled by:

    \snippet doc/src/snippets/code/src_network_access_FeatherWeightCache.cpp 0

    When sending requests, to control the preference of when to use the cache
    and when to use the network, consider the following:

    \snippet doc/src/snippets/code/src_network_access_FeatherWeightCache.cpp 1

    To check whether the response came from the cache or from the network, the
    following can be applied:

    \snippet doc/src/snippets/code/src_network_access_FeatherWeightCache.cpp 2
*/

/*!
    Creates a new disk cache. The \a parent argument is passed to
    QAbstractNetworkCache's constructor.
 */
FeatherWeightCache::FeatherWeightCache(QObject *parent)
    : QAbstractNetworkCache(parent)
{

    d = new FeatherWeightCachePrivate(this);
}

/*!
    Destroys the cache object.  This does not clear the disk cache.
 */
FeatherWeightCache::~FeatherWeightCache()
{
    QHashIterator<QIODevice*, CacheItem*> it(d->inserting);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }

}

/*!
    Returns the location where cached files will be stored.
*/
QString FeatherWeightCache::cacheDirectory() const
{
    return d->cacheDirectory;
}

/*!
    Sets the directory where cached files will be stored to \a cacheDir

    FeatherWeightCache will create this directory if it does not exists.

    Prepared cache items will be stored in the new cache directory when
    they are inserted.

    \sa QDesktopServices::CacheLocation
*/
void FeatherWeightCache::setCacheDirectory(const QString &cacheDir)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "FeatherWeightCache::setCacheDirectory()" << cacheDir;
#endif
    if (cacheDir.isEmpty())
        return;
    d->cacheDirectory = cacheDir;
    QDir dir(d->cacheDirectory);
    d->cacheDirectory = dir.absolutePath();
    if (!d->cacheDirectory.endsWith(QLatin1Char('/')))
        d->cacheDirectory += QLatin1Char('/');

    d->prepareLayout();
}

/*!
    \reimp
*/
qint64 FeatherWeightCache::cacheSize() const
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "FeatherWeightCache::cacheSize()";
#endif
    if (d->cacheDirectory.isEmpty())
        return 0;
    if (d->currentCacheSize < 0) {
        FeatherWeightCache *that = const_cast<FeatherWeightCache*>(this);
        that->d->currentCacheSize = that->expire();
    }
    return d->currentCacheSize;
}

/*!
    \reimp
*/
QIODevice *FeatherWeightCache::prepare(const QNetworkCacheMetaData &metaData)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::prepare()" << metaData.url();
#endif
    if (!metaData.isValid() || !metaData.url().isValid() || !metaData.saveToDisk())
        return 0;

    if (d->cacheDirectory.isEmpty()) {
        qWarning() << "FeatherWeightCache::prepare() The cache directory is not set";
        return 0;
    }

    foreach (QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders()) {
        if (header.first.toLower() == "content-length") {
            qint64 size = header.second.toInt();
            if (size > (maximumCacheSize() * 3)/4)
                return 0;
            break;
        }
    }
    QScopedPointer<CacheItem> cacheItem(new CacheItem);
    cacheItem->metaData = metaData;

    QIODevice *device = 0;
    if (cacheItem->canCompress()) {
        cacheItem->data.open(QBuffer::ReadWrite);
        device = &(cacheItem->data);
    } else {
        QString templateName = d->tmpCacheFileName();
        QT_TRY {
            cacheItem->file = new QTemporaryFile(templateName, &cacheItem->data);
        } QT_CATCH(...) {
            cacheItem->file = 0;
        }
        if (!cacheItem->file || !cacheItem->file->open()) {
            qWarning() << "FeatherWeightCache::prepare() unable to open temporary file";
            cacheItem.reset();
            return 0;
        }
        cacheItem->writeHeader(cacheItem->file);
        device = cacheItem->file;
    }
    d->inserting[device] = cacheItem.take();
    return device;
}

/*!
    \reimp
*/
void FeatherWeightCache::insert(QIODevice *device)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::insert()" << device;
#endif
    QHash<QIODevice*, CacheItem*>::iterator it = d->inserting.find(device);
    if (it == d->inserting.end()) {
        qWarning() << "FeatherWeightCache::insert() called on a device we don't know about" << device;
        return;
    }

    d->storeItem(it.value());
    delete it.value();
    d->inserting.erase(it);
}


/*!
    Create subdirectories and other housekeeping on the filesystem.
    Prevents too many files from being present in any single directory.
*/
void FeatherWeightCachePrivate::prepareLayout()
{
    QDir prepared;
    prepared.mkpath(cacheDirectory + PREPARED_SLASH);

    QString path = cacheDirectory + DATA_SLASH;
    QDir dataDirectory(path);

    //Create directory and subdirectories 0-F
    dataDirectory.mkpath(path);
    for ( uint i = 0; i < 16 ; i++ ) {
        QString str = QString::number(i, 16);
        QString subdir = dataDirectory.path() + QDir::separator() + str;
        dataDirectory.mkdir(subdir);
    }

    // TODO: populate volumeInfo members here base on which disk/fileystem
    // you plan to write (a) temp ("prepared") files to (b) write final cache files too
    // volumeInfo->clusterSize = 1024;
    // volumeInfo->readBufSize = 16384;
    // volumeInfo->writeBufSize = 16384;
#ifdef Q_OS_SYMBIAN
    //VolumeIOParam(TInt aDriveNo, TVolumeIOParamInfo &aParamInfo) const;
#endif
}

// CRC32 implementation.
// Could be made into new API QByteArray:qChecksum32()
static const quint32 crc_tbl32[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

quint32 FeatherWeightCachePrivate::crc32(const char *data, uint len)
{
    const uchar *p = reinterpret_cast<const uchar *>(data);
    const uchar *q = p + len;
    const quint32 init = 0xFFFFFFFFL;

    quint32 crc32 = init;
    while (p < q) {
        crc32 = (crc32 >> 8) ^ crc_tbl32[(crc32 ^ *p++) & 0xffL];
    }
    return crc32 ^ init ;
}

void FeatherWeightCachePrivate::storeItem(CacheItem *cacheItem)
{
    Q_ASSERT(cacheItem->metaData.saveToDisk());

    QString fileName = cacheFileName(cacheItem->metaData.url());
    Q_ASSERT(!fileName.isEmpty());


    if (currentCacheSize > 0) {
        currentCacheSize += FILESYSTEMOVERHEAD + cacheItem->size();
    }


    //lut.insert( URL2HASH(cacheItem->metaData.url()), FILESYSTEMOVERHEAD + cacheItem->size() ) ;

    currentCacheSize = (reinterpret_cast<FeatherWeightCache *>(parent()))->expire();



    if (!cacheItem->file) {
        QString templateName = tmpCacheFileName();
        cacheItem->file = new QTemporaryFile(templateName, &cacheItem->data);
        if (cacheItem->file->open()) {
            cacheItem->writeHeader(cacheItem->file);
            cacheItem->writeCompressedData(cacheItem->file);
        }
    }

    if (cacheItem->file
        && cacheItem->file->isOpen()
        && cacheItem->file->error() == QFile::NoError) {
        cacheItem->file->setAutoRemove(false);
        // ### use atomic rename rather then remove & rename
        if (cacheItem->file->rename(fileName))
            currentCacheSize += cacheItem->file->size();
        else {
            // Presume that the destination file exists and/or is open. So try nuking.
            bool err1 = QFile::remove(fileName);
            Q_UNUSED(err1);
            bool err2 = cacheItem->file->rename(fileName);
            // You are hopeless. Don't persist
            if (!err2)  {
                cacheItem->file->setAutoRemove(true);
#if defined(FEATHERWEIGHTCACHE_DEBUG)
                qWarning() << "FeatherWeightCache: couldn't replace the cache file " << fileName;
#endif
            }
        }
    }
    if (cacheItem->metaData.url() == lastItem.metaData.url())
        lastItem.reset();
}

/*!
    \reimp
*/
bool FeatherWeightCache::remove(const QUrl &url)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::remove()" << url;
#endif

    // remove is also used to cancel insertions, not a common operation
    QHashIterator<QIODevice*, CacheItem*> it(d->inserting);
    while (it.hasNext()) {
        it.next();
        CacheItem *item = it.value();
        if (item && item->metaData.url() == url) {
            delete item;
            d->inserting.remove(it.key());
            return true;
        }
    }

    if (d->lastItem.metaData.url() == url)
        d->lastItem.reset();
    return d->removeFile(d->cacheFileName(url));
}

/*!
    Put all of the misc file removing into one function to be extra safe
 */
bool FeatherWeightCachePrivate::removeFile(const QString &file)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::removFile()" << file;
#endif
    if (file.isEmpty())
        return false;
    QFileInfo info(file);
    QString fileName = info.fileName();
    if (!fileName.endsWith(CACHE_POSTFIX))
        return false;
    qint64 size = info.size();
    if (QFile::remove(file)) {
        currentCacheSize -= size;
        return true;
    }
    return false;
}

/*!
    Use signal from worker thread to update disk usage awareness
 */
void FeatherWeightCachePrivate::updateCacheSize(qint64 newSize)
{
    currentCacheSize = newSize;
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "FeatherWeightCachePrivate::updateCacheSize " << " new size " << currentCacheSize;
#endif
}

/*!
    \reimp
*/
QNetworkCacheMetaData FeatherWeightCache::metaData(const QUrl &url)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::metaData()" << url;
#endif
    if (d->lastItem.metaData.url() == url)
        return d->lastItem.metaData;
    return fileMetaData(d->cacheFileName(url));
}

/*!
    Returns the QNetworkCacheMetaData for the cache file \a fileName.

    If \a fileName is not a cache file QNetworkCacheMetaData will be invalid.
 */
QNetworkCacheMetaData FeatherWeightCache::fileMetaData(const QString &fileName) const
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::fileMetaData()" << fileName;
#endif
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return QNetworkCacheMetaData();
    if (!d->lastItem.read(&file, false)) {
        file.close();
        FeatherWeightCachePrivate *that = const_cast<FeatherWeightCachePrivate*>(d);
        that->removeFile(fileName);
    }
    return d->lastItem.metaData;
}

/*!
    \reimp
*/
QIODevice *FeatherWeightCache::data(const QUrl &url)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    //qDebug() << "FeatherWeightCache::data()" << url;
#endif

    QScopedPointer<QBuffer> buffer;
    if (!url.isValid())
        return 0;
    if (d->lastItem.metaData.url() == url && d->lastItem.data.isOpen()) {
        buffer.reset(new QBuffer);
        buffer->setData(d->lastItem.data.data());
    } else {
        QScopedPointer<QFile> file(new QFile(d->cacheFileName(url)));
        if (!file->open(QFile::ReadOnly | QIODevice::Unbuffered))
            return 0;

        if (!d->lastItem.read(file.data(), true)) {
            file->close();
            remove(url);
            return 0;
        }
        if (d->lastItem.data.isOpen()) {
            // compressed
            buffer.reset(new QBuffer);
            buffer->setData(d->lastItem.data.data());
        } else {
            buffer.reset(new QBuffer);
            // ### verify that QFile uses the fd size and not the file name
            qint64 size = file->size() - file->pos();
            const uchar *p = 0;
#ifndef Q_OS_WINCE
            p = file->map(file->pos(), size);
#endif
            if (p) {
                buffer->setData((const char *)p, size);
                file.take()->setParent(buffer.data());
            } else {
                buffer->setData(file->readAll());
            }
        }
    }
    buffer->open(QBuffer::ReadOnly);
    return buffer.take();
}

/*!
    \reimp
*/
void FeatherWeightCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "FeatherWeightCache::updateMetaData()" << metaData.url();
#endif
    QUrl url = metaData.url();
    QIODevice *oldDevice = data(url);
    if (!oldDevice) {
#if defined(FEATHERWEIGHTCACHE_DEBUG)
        qDebug() << "FeatherWeightCache::updateMetaData(), no device!";
#endif
        return;
    }

    QIODevice *newDevice = prepare(metaData);
    if (!newDevice) {
#if defined(FEATHERWEIGHTCACHE_DEBUG)
        qDebug() << "FeatherWeightCache::updateMetaData(), no new device!" << url;
#endif
        return;
    }
    //TODO: Optimize this somehow?
    char data[1024];
    while (!oldDevice->atEnd()) {
        qint64 s = oldDevice->read(data, 1024);
        newDevice->write(data, s);
    }
    delete oldDevice;
    insert(newDevice);
}

/*!
    Returns the current maximum size for the disk cache.

    \sa setMaximumCacheSize()
 */
qint64 FeatherWeightCache::maximumCacheSize() const
{
    return d->maximumCacheSize;
}

/*!
    Sets the maximum size of the disk cache to be \a size.

    If the new size is smaller then the current cache size then the cache will call expire().

    \sa maximumCacheSize()
 */
void FeatherWeightCache::setMaximumCacheSize(qint64 size)
{

    bool expireCache = (size < d->maximumCacheSize);
    d->maximumCacheSize = size;
    if (expireCache)
        d->currentCacheSize = expire();
}

/*!
    Cleans the cache so that its size is under the maximum cache size.
    Returns the current size of the cache.

    When the current size of the cache is greater than the maximumCacheSize()
    older cache files are removed until the total size is less then 90% of
    maximumCacheSize() starting with the oldest ones first using the file
    creation date to determine how old a cache file is.

    Subclasses can reimplement this function to change the order that cache
    files are removed taking into account information in the application
    knows about that FeatherWeightCache does not, for example the number of times
    a cache is accessed.

    Note: cacheSize() calls expire if the current cache size is unknown.

    \sa maximumCacheSize(), fileMetaData()
 */
qint64 FeatherWeightCache::expire()
{

    if (d->currentCacheSize >= 0 && d->currentCacheSize < maximumCacheSize())
        return d->currentCacheSize;

    if (cacheDirectory().isEmpty()) {
        qWarning() << "FeatherWeightCache::expire() The cache directory is not set";
        return 0;
    }

#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "Calling expire, size = " << d->currentCacheSize << " , max = " << maximumCacheSize() ;
#endif
    return d->expire();
}

/*!
    \reimp
*/
void FeatherWeightCache::clear()
{
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "FeatherWeightCache::clear()";
#endif

    qint64 size = d->maximumCacheSize;
    d->maximumCacheSize = 0;
    d->currentCacheSize = expire();
    d->maximumCacheSize = size;
}

qint64 FeatherWeightCachePrivate::expire()
{

    // ASYNC expiration via background thread
    beastOfBurden.expireLazily(cacheDirectory, maximumCacheSize);

    // Note: this cache size will not/cannot reflect the reduced
    // cache size due to the async nature of the expire() above.
    return currentCacheSize;
}

QByteArray FeatherWeightCachePrivate::generateId(const QUrl &url)
{
    QUrl cleanUrl = url;
    cleanUrl.setPassword(QString());
    cleanUrl.setFragment(QString());
    QByteArray blob = cleanUrl.toEncoded();

    QByteArray hash;
    hash.setNum(crc32(blob.data(), blob.length()), 16);
    return hash;
}

QString FeatherWeightCachePrivate::tmpCacheFileName() const
{
    //The subdirectory is presumed to be already read for use.
    return cacheDirectory + PREPARED_SLASH + QLatin1String("XXXXXX") + CACHE_POSTFIX;
}

/*!
    Genrates fully qualified path of cached resource from a URL.
 */
QString FeatherWeightCachePrivate::cacheFileName(const QUrl &url) const
{
    if (!url.isValid())
        return QString();

    // map URL to a unique enough signature
    const QByteArray unique(generateId(url));

    // generates <cache dir>/data/e/cache_beefcafe.cache
    // where 'e' is the last character of a hex string
    QString fullpath = cacheDirectory + DATA_SLASH
                       + QLatin1Char(unique.at(unique.length()-1)) + QLatin1String("/")
                       + QLatin1String(unique) + CACHE_POSTFIX;

    return  fullpath;
}


/* Important: This c'tor runs in the same thread as main cache */
WorkerThread::WorkerThread()
{
    abort = false;
}

/* Important: This d'tor runs in the same thread as main cache */
WorkerThread::~WorkerThread()
{
    // The destructor can be called at any point while the thread is active.
    // So we set abort to true to tell run() to stop running as soon as possible.
    mutex.lock();
    abort = true;
    condition.wakeOne(); // wake up thread if it has nothing to do
    mutex.unlock();

    wait(); // waits for run() to return
}

/* Important: This method runs in its own thread, unlike the c'tor and d'tor */
void WorkerThread::run()
{

#if defined(Q_OS_SYMBIAN)
    // Remove this once QTBUG-10271 is fixed
    RThread myThread;
    myThread.SetPriority(EPriorityLess);
#endif

    qint64 size = expireImpl();
    emit onDiskSizeChanged(size);

#if defined(FEATHERWEIGHTCACHE_DEBUG)
    qDebug() << "New on-disk cache size: " << size <<  QThread::currentThreadId();
#endif


}

qint64 WorkerThread::expireImpl()
{
    QDir::Filters filters = QDir::AllDirs | QDir:: Files | QDir::NoDotAndDotDot;
    QDirIterator it(this->cacheDir, filters, QDirIterator::Subdirectories);

    QMultiMap<QDateTime, QString> cacheItems;
    qint64 totalSize = 0;
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo info = it.fileInfo();
        QString fileName = info.fileName();
        if (fileName.endsWith(CACHE_POSTFIX)) {
            cacheItems.insert(info.created(), path);
            totalSize += info.size();
        }

        // Interrupts this slow loop when d'tor is called
        if (abort) {
            // potentially incorrect, but can't do any better
            return totalSize;
        }
    }

    int removedFiles = 0;
    // this goal setting could be made smarter based on max cache size
    // e.g on desktop with large 50MB caches, freeing 10% is probably enough
    // but on mobile where caches are smaller (e.g 5MB) and disks are slow, you want
    // to free atleast 0.5-1MB if going through all this trouble.
    // Also TODO: Move to LRU algorithm
    qint64 goal = (this->maxCacheSize * 8) / 10;
    QMultiMap<QDateTime, QString>::const_iterator i = cacheItems.constBegin();
    while (i != cacheItems.constEnd()) {
        if (totalSize < goal)
            break;
        QString name = i.value();
        QFile file(name);
        qint64 size = file.size();
        file.remove();
        totalSize -= size;
        ++removedFiles;
        ++i;

        // Interrupts this slow loop when d'tor is called
        if (abort) {
            // potentially incorrect, but can't do any better
            return totalSize;
        }

    }
#if defined(FEATHERWEIGHTCACHE_DEBUG)
    if (removedFiles > 0) {
        qDebug() << "FeatherWeightCache::expire()"
                << "Removed:" << removedFiles
                << "Kept:" << cacheItems.count() - removedFiles;
    }
#endif

    //TODO: Why do we do this in the original
    //implementation? It isn't necessary that
    //running expiration logics caused last
    //insertion to become invalid?
    //if (removedFiles > 0)
    //    lastItem.reset();

    return totalSize;

}

/* Important: this function runs in the same thread as main cache */
void WorkerThread::expireLazily(QString cacheDir, qint64 maxCacheSize)
{

    //lock mutex. unlock automatically when locker goes out of scope
    QMutexLocker locker(&mutex);

    //make private copy so that other member functions can use this
    this->cacheDir = cacheDir;
    this->maxCacheSize = maxCacheSize;

    if (!isRunning()) {

#if defined(FEATHERWEIGHTCACHE_DEBUG)
        qDebug() << "Starting worker thread a low priority";
#endif

        start(LowPriority);

    } else {

#if defined(FEATHERWEIGHTCACHE_DEBUG)
        qDebug() << "Waking up sleeping worker thread";
#endif

        condition.wakeOne();

    }

}


/*!
    We compress small text and JavaScript files.
 */
bool CacheItem::canCompress() const
{
#if 1
    bool sizeOk = false;
    bool typeOk = false;
    foreach (QNetworkCacheMetaData::RawHeader header, metaData.rawHeaders()) {
        if (header.first.toLower() == "content-length") {
            qint64 size = header.second.toLongLong();
            if (size > MAX_COMPRESSION_SIZE)
                return false;
            else
                sizeOk = true;
        }

        if (header.first.toLower() == "content-type") {
            QByteArray type = header.second;
            if (type.startsWith("text/")
                    || (type.startsWith("application/")
                        && (type.endsWith("javascript") || type.endsWith("ecmascript"))))
                typeOk = true;
            else
                return false;
        }
        if (sizeOk && typeOk)
            return true;
    }
    return false;
#else
    return false;
#endif

}

enum
{
    CacheMagic = 0xe8,
    CurrentCacheVersion = 7
};

void CacheItem::writeHeader(QFile *device) const
{
    QDataStream out(device);

    out << qint32(CacheMagic);
    out << qint32(CurrentCacheVersion);
    out << metaData;
    bool compressed = canCompress();
    out << compressed;
}

void CacheItem::writeCompressedData(QFile *device) const
{
    QDataStream out(device);

    out << qCompress(data.data());
}

/*!
    Returns false if the file is a cache file,
    but is an older version and should be removed otherwise true.
 */
bool CacheItem::read(QFile *device, bool readData)
{
    reset();

    QDataStream in(device);

    qint32 marker;
    qint32 v;
    in >> marker;
    in >> v;
    if (marker != CacheMagic)
        return true;

    // If the cache magic is correct, but the version is not we should remove it
    if (v != CurrentCacheVersion)
        return false;

    bool compressed;
    QByteArray dataBA;
    in >> metaData;
    in >> compressed;
    if (readData && compressed) {
        in >> dataBA;
        data.setData(qUncompress(dataBA));
        data.open(QBuffer::ReadOnly);
    }
    return metaData.isValid();
}

} // namespace WRT