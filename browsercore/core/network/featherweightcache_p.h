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

#ifndef FEATHERWEIGHTCACHE_P_H
#define FEATHERWEIGHTCACHE_P_H

#include <QBuffer>
#include <QHash>
#include <QTemporaryFile>
#include <QFile>
#include <QNetworkCacheMetaData>
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <cacheworkerthread.h>

namespace WRT {
class CacheItem
{
public:
    CacheItem() : file(0)
    {
    }
    ~CacheItem()
    {
        reset();
    }

    QNetworkCacheMetaData metaData;
    QBuffer data;
    QTemporaryFile *file;
    inline qint64 size() const
        { return file ? file->size() : data.size(); }

    inline void reset() {
        metaData = QNetworkCacheMetaData();
        data.close();
        delete file;
        file = 0;
    }
    void writeHeader(QFile *device) const;
    void writeCompressedData(QFile *device) const;
    bool read(QFile *device, bool readData);

    bool canCompress() const;
};


class WorkerThread : public QThread
{

    Q_OBJECT

public:
    WorkerThread();
    ~WorkerThread();

    void expireLazily(QString cacheDir, qint64 maxCacheSize);

protected:
    void run();

private:
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    qint64 expireImpl();

    QString cacheDir;
    qint64 maxCacheSize;

signals:
    void onDiskSizeChanged(qint64 newSize);

};


#define URL2HASH(url) FeatherWeightCachePrivate::generateId(url).toUInt()

// Assume this much higher physical disk usage. Platform and media dependent
// TODO: On Symbian, we could instead round up number to multiple of TVolumeIOParamInfo.iClusterSize
#define FILESYSTEMOVERHEAD 2048
#define ROUNDUPTOMULTIPLE(x, multipleof)    ( (x + multipleof - 1) & ~(multipleof - 1) )

class FeatherWeightCachePrivate : public QObject
{
    Q_OBJECT

public:
    FeatherWeightCachePrivate(QObject* parent): QObject(parent)
        , maximumCacheSize(1024 * 1024 * 10) //set the maximum default cache to 10M 
        , currentCacheSize(-1)
    {
        // Queued connection because onDiskSizeChanged() is currently always triggered from run() method of worker thread
        QObject::connect( &beastOfBurden, SIGNAL( onDiskSizeChanged(qint64) ), this, SLOT( updateCacheSize(qint64) ), Qt::DirectConnection );
    }

    ~FeatherWeightCachePrivate()
    {
        // When beastOfBurden's d'tor is called
        // it will wait() and then auto-terminate

        QObject::disconnect(&beastOfBurden, SIGNAL( onDiskSizeChanged(qint64) ), this, SLOT( updateCacheSize(qint64)));
    }

    qint64 expire();
    static QByteArray generateId(const QUrl &url);
    QString cacheFileName(const QUrl &url) const;
    QString tmpCacheFileName() const;
    bool removeFile(const QString &file);
    void storeItem(CacheItem *item);
    void prepareLayout();
    static quint32 crc32(const char *data, uint len);

    mutable CacheItem lastItem;
    QString cacheDirectory;
    qint64 maximumCacheSize;
    qint64 currentCacheSize;

    QHash<QIODevice*, CacheItem*> inserting;

    WorkerThread beastOfBurden;

    // a look up table (LUT) that stores file sizes
    // corresponding to cached URLs. Helps speed up
    // cache eviction or expiration logic.
    // key = CRC32 hash of url , value = size of file on disk
    // QHash<quint32, qint64> lut;

    // bool isTrimPending;

    //Recommended buffer sizes for fast IO on caching volume
    struct {
        quint32 readBufSize;
        quint32 writeBufSize;
        quint32 clusterSize;
    } volumeInfo;

public slots:
    void updateCacheSize(qint64);
};
}

#endif // FEATHERWEIGHTCACHE_P_H
