/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef BOOKMARKS_MANAGER_H
#define BOOKMARKS_MANAGER_H

#include <QtGui/QIcon>
#include <QObject>
#include <QAction>
#include "BWFGlobal.h"
#include "bedrockprovisioning.h"

int MainImport();

namespace WRT {
int startNativeBookmarksParsing();

class BookmarksManagerPrivate;
class BookmarkNode;
class HistoryModel;

class BWF_EXPORT BookmarksManager : public QObject {
  
  Q_OBJECT
  
  enum EBookmarkManagerErrorTypes
         {
          ErrNone,
      //Add bookmark failure
      ErrGeneral = -1,
          ErrBookmarkAllReadyPresent =-2,
      ErrBookmarkUrlEmpty   = -3,
          
          };

  public:
    BookmarksManager(QWidget *parent = 0);
    ~BookmarksManager();

    void setSettings(BEDROCK_PROVISIONING::BedrockProvisioning *settings);
    //Gets ref count of the page from history
    int getPageRank(const QString &url);
    static BookmarksManager* getSingleton();
    
  signals:
    void bookmarkEntryAdded(QString,QString);
    void launchBookmarkEditDailog(QString,QString);
    void bookmarkEntryModified(QString, QString);
    void historyCleared();
    void confirmHistoryClear();
    void bookmarksCleared();

    public slots:

#ifdef Q_WS_MAEMO_5
    void importNativeBookmarks();
#endif

    //javascript APIS
    QString getBookmarksJSON();
    int addBookmark(const QString &title,const QString &url,int index=0);
    void deleteBookmark(QString title);
    int reorderBokmarks(QString title,int new_index);
    int modifyBookmark(QString orgTitle, QString newTitle, QString newUrl);
    void clearBookmarks();
    void launchEditBookmark(QString title,QString url);

    QString getHistoryFoldersJSON(QString folder="");
    void addHistory(const QString &url, const QString &title);
    void addHistory(const QUrl &url, const QString &title);
    void clearHistory();

    QAction * getActionClearHistory();

  private:
    //For loading the history from data base
    QString normalizeUrl(const QString &url);

  private:
     BookmarksManagerPrivate * const d;
     bool m_isBookmarkDbreadRequired;
     QString m_bookmakrData;
     QVector<QString> m_folderVector;
     QMap<QString, QString> m_historyMap;
     
};
}
#endif //BOOKMARKS_MANAGER
