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


#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <QtCore/QObject>

//#include <QtGui/QUndoCommand>
#include <QtGui/QIcon>
#include <QtCore/QDate>

#include "wrttypes.h"
#include "BWFGlobal.h"
#include <browsercontentdll.h>

namespace WRT {

class AutoSaver;
class BookmarksModel;
//class WrtSettings;
class BookmarksManager;

class BWF_EXPORT BookmarkNode
{
public:
    /*!
     * Enumeration representing  node type
     */
    enum Type {
        /**Root node */
        Root,
        /** Folder node */
        Folder,
        /** Bookmark  */
        Bookmark,
        /** Seperator */
        Separator
    };

    BookmarkNode(Type type = Root, BookmarkNode *parent = 0);
    ~BookmarkNode();
    bool operator==(const BookmarkNode &other);

    Type type() const;
    void setType(Type type);
    QList<BookmarkNode *> children() const;
    BookmarkNode *parent() const;

    bool isBookmark() const;
    bool isFolder() const;

    void add(BookmarkNode *child, int offset = -1);
    void remove(BookmarkNode *child);
    QString getUrl(){return url;}
    QString getTitle(){return title;}
    QDate getDate(){return date;}
    QTime getLastVisited(){return lastVisited;}
    QString getDesc(){return desc;}
    QIcon getFavIcon(){return favicon;}
    QString getTag(){ return m_tag;}

    void setUrl(QString aurl){url=aurl;}
    void setTitle(QString atitle){title=atitle;}
    void setDate(QDate adate){date=adate;}
    void setLastVisited(QTime atime){lastVisited=atime;}
    void setDesc(QString adesc){desc=adesc;}
    void setFavIcon(QIcon afavicon){favicon=afavicon;}
    void setTag(QString atag){m_tag=atag;}
    
    QString url;
    QString title;

    /*The last date on which URL is visited*/
    QDate date;
    /*The last time on which URL is visited*/
    QTime lastVisited;

    QString desc;
    QIcon favicon;
    bool expanded;
    QString m_tag;
    

private :
   /**
    * Hide copy constructors and assignment operators
    */
    BookmarkNode(BookmarkNode &aNode);
    BookmarkNode & operator = (BookmarkNode &aNode);  
    
private :
    BookmarkNode *m_parent;
    Type m_type;
    QList<BookmarkNode *> m_children;
    

};

}
#endif // BOOKMARKS_H
