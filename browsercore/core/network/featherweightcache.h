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

#ifndef FEATHERWEIGHTCACHE_H
#define FEATHERWEIGHTCACHE_H

# include <QAbstractNetworkCache>
# include <QNetworkCacheMetaData>
# include <QDir>
# include <QFile>
# include <QtGlobal>

#include "brtglobal.h"

namespace WRT {
class FeatherWeightCachePrivate;
class WRT_BROWSER_EXPORT FeatherWeightCache : public QAbstractNetworkCache
{
    Q_OBJECT

public:
    explicit FeatherWeightCache(QObject *parent = 0);
    ~FeatherWeightCache();

    QString cacheDirectory() const;
    void setCacheDirectory(const QString &cacheDir);

    qint64 maximumCacheSize() const;
    void setMaximumCacheSize(qint64 size);

    qint64 cacheSize() const;
    QNetworkCacheMetaData metaData(const QUrl &url);
    void updateMetaData(const QNetworkCacheMetaData &metaData);
    QIODevice *data(const QUrl &url);
    bool remove(const QUrl &url);
    QIODevice *prepare(const QNetworkCacheMetaData &metaData);
    void insert(QIODevice *device);

    QNetworkCacheMetaData fileMetaData(const QString &fileName) const;

public Q_SLOTS:
    void clear();

protected:
     qint64 expire();

private:
    FeatherWeightCachePrivate* d;
    friend class FeatherWeightCachePrivate;
    Q_DISABLE_COPY(FeatherWeightCache)
};
} // namespace WRT

#endif // FEATHERWEIGHTCACHE_H
