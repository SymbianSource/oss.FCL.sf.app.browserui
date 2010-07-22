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


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QString>
#include "actionjsobject.h"
#include "BookmarksManager_p.h"
#include "BookmarksManager.h"

//#include "wrtsettings.h"
#include "bedrockprovisioning.h"
#include "bookmarks.h"
#include <browsercontentdll.h>
#include "xbel.h"
#include "webpagecontroller.h"
#include "wrtbrowsercontainer.h"


namespace WRT {

BookmarksManagerPrivate::BookmarksManagerPrivate(BookmarksManager * mgr, QWidget *parent) :
    q(mgr),
    m_connectedToBookmarks(false),
    m_maxUrls(10) // TODO: read from settings
{
    m_import = false;
    QFileInfo dbFile("browserContent.db");
    
    if (dbFile.exists()){
      m_import = false;
    }
    else {
      m_import = true;
    }

    m_bookmarkSession=new BrowserContent("Bedrock");
    if (m_bookmarkSession) {
        m_connectedToBookmarks = true;
    } else {
        qDebug() << "BookmarksManagerPrivate: Failed to connect to bookmarks";
    }

    m_actionsParent = new QObject(mgr);
    m_actionsParent->setObjectName("actions");
    
    m_actionClearHistory = new QAction("clearHistory", m_actionsParent);
    
    m_actionClearJSO  = new ActionJSObject(m_actionsParent, m_actionClearHistory);
    
    m_actionClearHistory->setObjectName("clearHistory");

}

BookmarksManagerPrivate::~BookmarksManagerPrivate()
{
    delete m_bookmarkSession;
    delete m_actionClearHistory;
    delete m_actionClearJSO;
}

/*!
 * \class BookmarksManager
 *
 * This class is responsible for managing bookmarks, This class could be used 
 * mainly for Creating, adding bookmarks, adding recent history items, deleting 
 * and editing bookmarks.
 *
 */
 
/*!
 * Basic constructor
 * @param parent : parent widget (Defaulted to NULL ) if not specified
 */
BookmarksManager::BookmarksManager(QWidget *parent) :
    d(new BookmarksManagerPrivate(this, parent))
{
    if (d->m_import)
       importNativeBookmarks();
    
    m_isBookmarkDbreadRequired=true;
    //connect(d->m_actionClearHistory, SIGNAL(triggered()), this, SIGNAL(historyCleared()));
    connect(d->m_actionClearHistory, SIGNAL(triggered()), this, SIGNAL(confirmHistoryClear()));
     
}
/*
void BookmarksManager::actionClearHistory()
{
    emit confirmClearHistory();
}
*/

BookmarksManager::~BookmarksManager()
{
    disconnect(d->m_actionClearHistory, SIGNAL(triggered()), this, SIGNAL(historyCleared()));
    delete d;
}

BookmarksManager* BookmarksManager::getSingleton()
 {
    static BookmarksManager* singleton = 0;
  
    if(!singleton){
           singleton = new BookmarksManager;
           singleton->setObjectName("bookmarksManager");
    }

    //assert(singleton);
    return singleton;    
 }
}

/*!
 * Import the bookmarks grom Native Browser
 * Ignores errors (just exists)
 */
void BookmarksManager::importNativeBookmarks()
{
#ifdef Q_OS_SYMBIAN
             
      int error = ::MainImport();
      
      if(error)
          return;
          
    startNativeBookmarksParsing();
    QString fileName = "c:/data/bookmarks.txt.xml";
    
    XbelReader reader;
    BookmarkNode *importRootNode = reader.read(fileName);
    if (reader.error() != QXmlStreamReader::NoError){
        QMessageBox::warning(0, QLatin1String("Loading Bookmark"),
            tr("Error when loading bookmarks on line %1, column %2:\n"
               "%3").arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString()));
    }

    int bookmarkCount = importRootNode->children().count() - 1;
    for (int i = bookmarkCount; i >= 0; --i) {
        BookmarkNode *node = importRootNode->children()[i];

        if (node->type() == BookmarkNode::Bookmark) 
            addBookmark(node->title,node->url,0);
        
    } 
#endif
    // add local bookmarks
    QString localPagesBaseDir(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("LocalPagesBaseDirectory"));
    QString indexStr;
    for (int index = 2; index >= 0; index--) {
        indexStr.setNum(index);
        QString bookmarkTitle = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString(tr("Bookmark")+indexStr+tr("Title"));
        if (bookmarkTitle == "")
            continue;
        QString bookmarkUrl = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString(tr("Bookmark")+indexStr+tr("Url"));
        if (bookmarkUrl == "")
            continue;
        if (bookmarkUrl.contains("://") || bookmarkUrl.contains("www."))
            addBookmark(bookmarkTitle, bookmarkUrl, 0);
        else {
            QString localBookmarkUrl = QFileInfo(localPagesBaseDir + bookmarkUrl).absoluteFilePath();
            addBookmark(bookmarkTitle, tr("file:///") + localBookmarkUrl, 0);
        }
    }
}

QString BookmarksManager::getBookmarksJSON()
    {
    if(m_isBookmarkDbreadRequired==true)
        {
    m_bookmakrData=d->m_bookmarkSession->fetchSerializedBookmarks();
    m_isBookmarkDbreadRequired=false;
       }
    return m_bookmakrData;

    }

QString BookmarksManager::normalizeUrl(const QString& url)
    {
    // If the URL is relative, add http in front
    // so that qt doesn't make it absolute using the
    // local file path
    QString updatedUrl = url;
    
    if (!url.contains("://")) {
        if (!url.startsWith("www", Qt::CaseInsensitive)) {
            updatedUrl.prepend("http://www.");
        } else {
           updatedUrl.prepend("http://");
        }
    }
    return updatedUrl;
    
    }

int BookmarksManager::addBookmark(const QString &title, const QString &url,int index)
{
    QString updatedTitle = title;
	updatedTitle = updatedTitle.trimmed();
    //Setting title as url string if title is not available
    if(url.isEmpty())
        return ErrBookmarkUrlEmpty;
    
    QVector<QString> nodes;
    d->m_bookmarkSession->fetchAllBookmarkTitles(nodes);
    for (int iter = 0 ; iter < nodes.size() ; ++iter)   {
		if(0==updatedTitle.compare(nodes.at(iter),Qt::CaseInsensitive)){
      //Node already exists delete existing
		 deleteBookmark(updatedTitle); 
      }
    }

   
    nodes.clear();
    
    QString updatedUrl = normalizeUrl(url);
    
    //Setting title as url string if title is not available
    if(title.isEmpty())
        updatedTitle = updatedUrl;
    
    BookmarkLeaf* leaf=new BookmarkLeaf();
    leaf->setTitle(updatedTitle);
    leaf->setUrl(updatedUrl);
    //leaf->setDate(bookmark->date);
    leaf->setTag("unknown");
    leaf->setIndex(index);
    
    if(ErrNone == d->m_bookmarkSession->addBookmark(leaf)) {
    emit bookmarkEntryAdded(updatedTitle,updatedUrl);
    delete leaf;
    m_isBookmarkDbreadRequired=true;
    return ErrNone;
    }
    delete leaf;
    return ErrGeneral;
}

void BookmarksManager::deleteBookmark(QString title)
{
    d->m_bookmarkSession->deleteBookmark(title);
    m_isBookmarkDbreadRequired=true;
}

void BookmarksManager::clearBookmarks()
{
    d->m_bookmarkSession->clearBookmarks();
    m_isBookmarkDbreadRequired=true;
    emit bookmarksCleared();
}

int BookmarksManager::reorderBokmarks(QString title,int new_index)
{
    d->m_bookmarkSession->reorderBokmarks(title,new_index);
    m_isBookmarkDbreadRequired=true;
    return ErrNone;    
}

int BookmarksManager::modifyBookmark(QString orgTitle, QString newTitle, QString newUrl)
{
    QString updatedTitle = newTitle;
	updatedTitle = updatedTitle.trimmed();
    //Setting title as url string if title is not available
    if(newUrl.isEmpty())
        return ErrBookmarkUrlEmpty;
    
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->fetchAllBookmarks();
    for (int iter = 0 ; iter < nodes.count() ; ++iter)   {
     if((0==updatedTitle.compare(nodes.at(iter)->getTitle(),Qt::CaseInsensitive))
         && (0 != orgTitle.compare(nodes.at(iter)->getTitle(),Qt::CaseInsensitive))){
         //Node already exists delete existing
         deleteBookmark(updatedTitle);
      }
    }
    
    qDeleteAll(nodes);
    nodes.clear();

   QString updatedUrl = normalizeUrl(newUrl);
   
  //Setting title as url string if title is not available
    if(newTitle.isEmpty())
        updatedTitle = updatedUrl;


    if(ErrNone == d->m_bookmarkSession->modifyBookmark(orgTitle, updatedTitle, updatedUrl)) {
        emit bookmarkEntryModified(updatedTitle, updatedUrl);
        m_isBookmarkDbreadRequired=true;
        return ErrNone;
    }
    return ErrGeneral;
}

void BookmarksManager::launchEditBookmark(QString title,QString url)
{
    emit launchBookmarkEditDailog(title,url);
}

QString BookmarksManager::getHistoryFoldersJSON(QString folderName)
    {

    bool flag = true;
  
    QString historyFolders = "[";
    if (folderName == "")
        {
    m_historyMap.clear();
    m_folderVector.clear();
    d->m_bookmarkSession->fetchSerializedHistory(m_folderVector, m_historyMap);

    for (int i = m_folderVector.size() - 1; i >= 0; i--)
        {
        //check for folder nodes
        historyFolders.append("\"");
        historyFolders.append(m_folderVector[i]);
        historyFolders.append("\"");


        if (i != 0)
            historyFolders.append(",");
        if (flag)
            {
            if (m_folderVector[i].count() > 0)
                {
                d->m_actionClearHistory->setEnabled(true);
                flag=false;
                }

            }
        }
     if (flag)
        {
        d->m_actionClearHistory->setEnabled(false);
        }
    historyFolders.append("]");
    m_folderVector.clear();
        }
    if (folderName == "")
        {
        return historyFolders;

        }
    else
        {
        return m_historyMap[folderName];
        }
    }

/*!
 * Add the  node to the folder in proxy model. If the folder doesnt exist in proxy, create
 * the folder  and add the node to it
 */

/*!
 * Add to recent urls
 * @param url: url to be added to recent url list
 * @param title: title for this item in recent url list
 * @param icon: icon to be added for this item in recent url list
 */
Q_DECL_EXPORT void BookmarksManager::addHistory(const QString &url, const QString &title)
{

    //Check for a valid history entry
    if (url.isEmpty() || title.isEmpty())
        return;
    
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("SaveHistory");
    if(!enabled)
      return;
    
    QDateTime currentDateTime = QDateTime::currentDateTime();
    
    HistoryLeaf* leaf=new HistoryLeaf();
    leaf->setTitle(title);
    leaf->setUrl(url);
    leaf->setDate(currentDateTime.date());
    leaf->setLastVisited(currentDateTime.time());
    
    if (d->m_connectedToBookmarks){
        if(ErrNone == d->m_bookmarkSession->addHistory(leaf)){
          d->m_actionClearHistory->setEnabled(true);
        }
    }
    delete leaf;
}

/*!
 * Add to recent urls
 * @param url: url to be added to recent url list
 * @param title: title for this item in recent url list
 * @param icon: icon to be added for this item in recent url list
 */

void BookmarksManager::addHistory(const QUrl &url, const QString &title)
{
    addHistory(url.toString(), title);
}


/*!
 * delete recent urls
 * clears all the recent url list.
 */
void BookmarksManager::clearHistory()
{

    if (d->m_connectedToBookmarks) {
        d->m_bookmarkSession->clearHistory();
    }
    
    d->m_actionClearHistory->setEnabled(false);
    
    emit historyCleared();
}

QAction * BookmarksManager::getActionClearHistory()
{
    return d->m_actionClearHistory;
}

int BookmarksManager::getPageRank(const QString &url)
{
 //Check for a valid entry
    if (url.isNull())
        return 0;

    int rank = 0;
	QList<HistoryLeaf*> historyNodes = d->m_bookmarkSession->fetchHistory();

    for (int i=0; i < historyNodes.count(); i++) {
      //Update rank if there is a history for this URL.
        if (!historyNodes[i]->getUrl().compare(url))
           rank++;
    }

	while (!historyNodes.isEmpty())
     delete historyNodes.takeFirst();

    return rank;
}
