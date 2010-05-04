/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef XBEL_H
#define XBEL_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/QDateTime>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QtCore/QXmlStreamWriter>
#include "wrttypes.h"

namespace WRT {

class BookmarkNode;

class XbelReader : public QXmlStreamReader
{
public:
    XbelReader();
    BookmarkNode *read(const QString &fileName);
    BookmarkNode *read(QIODevice *device);

private:
    void skipUnknownElement();
    void readXBEL(BookmarkNode *parent);
    void readTitle(BookmarkNode *parent);
    void readDate(BookmarkNode *parent);
    void readLastVisted(BookmarkNode *parent);
    void readDescription(BookmarkNode *parent);
    void readFavicon(BookmarkNode *parent);
    void readSeparator(BookmarkNode *parent);
    void readFolder(BookmarkNode *parent);
    void readBookmarkNode(BookmarkNode *parent);
};


class XbelWriter : public QXmlStreamWriter
{
public:
    XbelWriter();
    bool write(const QString &fileName, const BookmarkNode *root);
    bool write(QIODevice *device, const BookmarkNode *root);

private:
    void writeItem(const BookmarkNode *parent);
    void writeImageData(const QImage &img);
};
}
#endif // XBEL_H

