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


#ifndef BOOKMARKS_MANAGER_H
#define BOOKMARKS_MANAGER_H

#include <QtGui/QIcon>
#include <QObject>
#include "BWFGlobal.h"
#include "bedrockprovisioning.h"

int MainImport();

namespace WRT {
	  int startNativeBookmarksParsing();
  
    class BookmarksManagerPrivate;
    class BookmarkNode;
    class HistoryModel;

    class BWF_EXPORT BookmarksManager : public QObject
    {
		enum EBookmarkManagerErrorTypes
         {
          ErrNone,
		  //Add bookmark failure
		  ErrGeneral = -1,
          ErrBookmarkAllReadyPresent =-2,
		  ErrBookmarkUrlEmpty   = -3,
          
          };

        Q_OBJECT
    public:
        BookmarksManager(QWidget *parent = 0);
        ~BookmarksManager();

        void setSettings(BEDROCK_PROVISIONING::BedrockProvisioning *settings);
        //Gets ref count of the page from history
        int getPageRank(const QString &url);
        
        static BookmarksManager* getSingleton();
        
    
    signals:
        
    public slots:
		
        void importNativeBookmarks();
		//javascript APIS
		QString getBookmarksJSON();
		int addBookmark(const QString &url, const QString &title,int index=0);
		void deleteBookmark(QString title);
		int reorderBokmarks(QString title,int new_index);		
        
        QString getHistoryFoldersJSON();
        QString getHistoryFolderJSON(QString folder);
        void addHistory(const QString &url, const QString &title);
        void addHistory(const QUrl &url, const QString &title);
        void clearHistory();
        
    private:
        //For loading the history from data base
        void loadHistory();
        void loadHistoryProxy();
        //Finds the folder to which the date belongs
        QString findFolderForDate( QDate& date) const;
        //add the node to history proxy
        void addToHistoryProxy(QString &strFoldername, BookmarkNode*node);
        //add the history item to root node specified
        bool dateInThisMonth(QDate &date)const;

    private:
        BookmarksManagerPrivate * const d;
    };
}
#endif //BOOKMARKS_MANAGER
