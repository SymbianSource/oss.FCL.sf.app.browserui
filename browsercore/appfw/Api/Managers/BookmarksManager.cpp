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
    m_loadedBookmarks(false),
    m_loadedHistory(false),
    m_bookmarkRootNode(0),
    m_historyRootNode(0),
    m_historyRootNodeProxy(0),
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
    delete m_bookmarkRootNode;
    delete m_historyRootNode;
    delete m_historyRootNodeProxy;
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
    // always add Bedrock Browser Welcome Page
    QString chromeBaseDir(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("ChromeBaseDirectory"));
    QString welcomeUrl(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("StartPage"));
    QString welcomePage = chromeBaseDir + welcomeUrl;
    addBookmark("Bedrock Welcome Page",welcomePage,0);
     
}

QString BookmarksManager::getBookmarksJSON()
    {
    if(m_isBookmarkDbreadRequired==true)
        {
    m_bookmakrData=d->m_bookmarkSession->FetchSerializedBookmarks();
    m_isBookmarkDbreadRequired=false;
       }
    return m_bookmakrData;

    }

int BookmarksManager::addBookmark(const QString &title, const QString &url,int index)
{
    QString updatedTitle = title;
    QString updatedUrl = url;
    //Setting title as url string if title is not available
    if(url.isEmpty())
        return ErrBookmarkUrlEmpty;
    
    QVector<QString> nodes;
    d->m_bookmarkSession->FetchAllBookmarkTitles(nodes);
    for (int iter = 0 ; iter < nodes.size() ; ++iter)   {
     if(nodes.at(iter) == title)   {
        //Node already exists no need to add, just return
        return ErrBookmarkAllReadyPresent;
      }
    }

   
    nodes.clear();
    
    // If the URL is relative, add http in front
    // so that qt doesn't make it absolute using the
    // local file path
    if (!url.contains("://")) {
        if (!url.startsWith("www", Qt::CaseInsensitive)) {
            updatedUrl.prepend("http://www.");
        } else {
           updatedUrl.prepend("http://");
        }
    }

    //Setting title as url string if title is not available
        if(title.isEmpty())
            updatedTitle = updatedUrl;
    
    BookmarkLeaf* leaf=new BookmarkLeaf();
    leaf->setTitle(updatedTitle);
    leaf->setUrl(updatedUrl);
    //leaf->setDate(bookmark->date);
    leaf->setTag("unknown");
    leaf->setIndex(index);
    
    if(ErrNone == d->m_bookmarkSession->AddBookmark(leaf)) {
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
    d->m_bookmarkSession->DeleteBookmark(title);
    m_isBookmarkDbreadRequired=true;
}

void BookmarksManager::clearBookmarks()
{
    d->m_bookmarkSession->clearBookmarks();
    m_isBookmarkDbreadRequired=true;
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
    //Setting title as url string if title is not available
    if(newUrl.isEmpty())
        return ErrBookmarkUrlEmpty;
    
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->FetchAllBookmarks();
    for (int iter = 0 ; iter < nodes.count() ; ++iter)   {
     if((nodes.at(iter)->getTitle() == newTitle) && (nodes.at(iter)->getTitle() != orgTitle))   {
        //Node already exists no need to add, just return
        return ErrBookmarkAllReadyPresent;
      }
    }
    
    qDeleteAll(nodes);
    nodes.clear();

  //Setting title as url string if title is not available
    if(newTitle.isEmpty())
        updatedTitle = newUrl;


    if(ErrNone == d->m_bookmarkSession->modifyBookmark(orgTitle, newTitle, newUrl)) {
        emit bookmarkEntryModified(newTitle, newUrl);
        m_isBookmarkDbreadRequired=true;
        return ErrNone;
    }
    return ErrGeneral;
}

void BookmarksManager::launchEditBookmark(QString title,QString url)
{
    emit launchBookmarkEditDailog(title,url);
}

/*!
 * Load history from database
 */
void BookmarksManager::loadHistory()
{
    if (d->m_loadedHistory)
        return;

    d->m_loadedHistory = true;

    if(d->m_historyRootNode) {
        delete d->m_historyRootNode;
        d->m_historyRootNode = NULL;
    }

    d->m_historyRootNode = new BookmarkNode(BookmarkNode::Root, NULL);

    if (d->m_connectedToBookmarks) {
        QList<HistoryLeaf*> nodes;
        nodes = d->m_bookmarkSession->FetchHistory();

        for(int i=nodes.count()-1;i>=0;i--) {
            BookmarkNode* node = new BookmarkNode(BookmarkNode::Bookmark, NULL);
            node->title=nodes[i]->getTitle();
            node->url=nodes[i]->getUrl();
            node->date=nodes[i]->getDate();
            node->lastVisited =nodes[i]->getLastVisited();
            d->m_historyRootNode->add(node, 0);
        }

    qDeleteAll(nodes);
    nodes.clear();
    }

       loadHistoryProxy();
}

/*!
 * Load history proxy from existing database
 * This creates new Model class based on existing Model and arranges the items in to groups
 * "Today", "YesterDay", "This Week",  "This Month" ......... 
 */
void BookmarksManager::loadHistoryProxy()
{
    if(d->m_historyRootNodeProxy) {
        delete d->m_historyRootNodeProxy;
        d->m_historyRootNodeProxy = NULL;
    }
    
    d->m_historyRootNodeProxy  = new BookmarkNode(BookmarkNode::Root, NULL);

    //Loop through the root elements and find the folder in proxy to which the elements belong     
    for (int i = d->m_historyRootNode->children().count() - 1; i >= 0; --i) {
        
        BookmarkNode *node = d->m_historyRootNode->children()[i];
        int daysToCurrentDate = node->date.daysTo(QDate::currentDate());

        if(daysToCurrentDate < 0) {
            continue;
        }
        //Find the Folder name to which this node belongs
        QString strFoldername =  findFolderForDate(node->date);
        //Add the  node to proxy model
        addToHistoryProxy(strFoldername,node);
    }                
}

QString BookmarksManager::getHistoryFoldersJSON(QString folderName)
    {
    QVector<QString> folderVector;
    QMap<QString, QString> mymap;
    bool flag = true;
    mymap.clear();
    folderVector.clear();
    d->m_bookmarkSession->FetchSerializedHistory(folderVector, mymap);
    QString historyFolders = "[";
    for (int i = folderVector.size() - 1; i >= 0; i--)
        {
        //check for folder nodes
        historyFolders.append("\"");
        historyFolders.append(folderVector[i]);
        historyFolders.append("\"");


        if (i != 0)
            historyFolders.append(",");
        if (flag)
            {
            if (folderVector[i].count() > 0)
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
    folderVector.clear();
    if (folderName == "")
        {
        return historyFolders;

        }
    else
        {
        return mymap[folderName];
        }
    }

/*!
 * Add the  node to the folder in proxy model. If the folder doesnt exist in proxy, create
 * the folder  and add the node to it
 */
void BookmarksManager::addToHistoryProxy(QString &strFoldername, BookmarkNode*node)
{
    bool bFound = false;
    BookmarkNode *folderNode= NULL;
    
    //Check if the folder exist with name $strFoldername
    for (int i = d->m_historyRootNodeProxy->children().count() - 1; i >= 0; --i) {
               folderNode = d->m_historyRootNodeProxy->children()[i];
               QString title = folderNode->title ;
               if(folderNode->title == strFoldername){
               //folder exist
                   bFound = true;
                   break;
               }         
    }

    if(!bFound){
        //Folder doesnt exist. create new folder with name $strFoldername
        folderNode = new BookmarkNode(BookmarkNode::Folder, d->m_historyRootNodeProxy);
        folderNode->title = strFoldername;
        folderNode->date = node->date;
        
        int index=0;
        //Find the index to where the folder needs to be inserted
        for (; index < d->m_historyRootNodeProxy->children().count(); index++) {
           BookmarkNode* tmpNode = d->m_historyRootNodeProxy->children()[index];
           if(folderNode->date  > tmpNode->date){
             break;
           }  
        }
        d->m_historyRootNodeProxy->add(folderNode,index);
    }
    
    

    BookmarkNode *newNode = new BookmarkNode(BookmarkNode::Bookmark,folderNode);
    newNode->date = node->date;
    newNode->title = node->title;
    newNode->lastVisited = node->lastVisited;
    newNode->favicon = node->favicon;
    newNode->url = node->url;
    
    //Find the index to where the node needs to be inserted in to the foder
    int index=0;
    for (; index < folderNode->children().count() ;  index++) {
       BookmarkNode* tmpNode = folderNode->children()[index];
            if(newNode->lastVisited  > tmpNode->lastVisited){
                 break;
               }
    }
    
    folderNode->add(newNode,index);
}


/*!
 * Finds the folder to which the date belongs
 * 
 */
//QString BookmarksManager::findFolderForDate( BookmarkNode *aNode)
QString BookmarksManager::findFolderForDate( QDate& nodeDate)const
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int currentDayOfWeek = currentDateTime.date().dayOfWeek(); 

    int nodeDayOfWeek = nodeDate.dayOfWeek();

    int daysToCurrentDate = nodeDate.daysTo(currentDateTime.date());

    //Check if date to belongs to "ToDay" Folder
    if(nodeDate == currentDateTime.date()){
        QString folder = qtTrId("txt_browser_history_today");
        return folder;
    }
    //Check if date to belongs to "YesterDay" Folder
    if(nodeDate.addDays(1) == currentDateTime.date() ){
        QString folder = qtTrId("txt_browser_history_yesterday");
        return folder;
    }

    //Check if date to belongs to current week folder
    //Should disply the day for the current week
    if(daysToCurrentDate < 7  &&  currentDayOfWeek > nodeDayOfWeek ){
        
        QString folder = qtTrId("txt_browser_history_this_week");
        return folder;
    }

    if(dateInThisMonth(nodeDate)){
        QString folder = qtTrId("txt_browser_history_this_month");
        return folder;
    }

    QString folder = nodeDate.toString("dd.MM.yyyy");
    return folder; 

}



bool BookmarksManager::dateInThisMonth(QDate &date)const
{
    QDate currentDate = QDateTime::currentDateTime().date();
    int daysToCurrentDate = currentDate.daysTo(date);

    int currentMonth = currentDate.month();
    int nodeMonth = date.month();

    if(daysToCurrentDate <= 31 && currentMonth == nodeMonth) {
        return true;
    }
    return false;  
   
}


/*!
 * Add to recent urls
 * @param url: url to be added to recent url list
 * @param title: title for this item in recent url list
 * @param icon: icon to be added for this item in recent url list
 */
Q_DECL_EXPORT void BookmarksManager::addHistory(const QString &url, const QString &title)
{
    if (!d->m_loadedHistory)
        loadHistory();

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
        if(ErrNone == d->m_bookmarkSession->AddHistory(leaf)){
          d->m_actionClearHistory->setEnabled(true);
          d->m_loadedHistory = false;
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
    if (!d->m_loadedHistory)
        loadHistory();

    //Undo Redo is not required for history(recent urls)
    for (int i = d->m_historyRootNode->children().count() - 1; i >= 0; --i) {
        BookmarkNode *node = d->m_historyRootNode->children()[i];     
        d->m_historyRootNode->remove(node);
    }
    
    if (d->m_connectedToBookmarks) {
        d->m_bookmarkSession->clearHistory();
    }
    d->m_loadedHistory = false;
    
    d->m_actionClearHistory->setEnabled(false);
    
    emit historyCleared();
}

QAction * BookmarksManager::getActionClearHistory()
{
    return d->m_actionClearHistory;
}

int BookmarksManager::getPageRank(const QString &url)
{
    if (!d->m_loadedHistory)
        loadHistory();

    //Check for a valid entry
    if (url.isNull())
        return 0;

    int rank = 0;
    QList<BookmarkNode *> rootChildren (d->m_historyRootNode->children());

    for (int i=0; i < rootChildren.count(); i++) {
      //Update rank if there is a history for this URL.
        if (!rootChildren[i]->getUrl().compare(url))
           rank++;
    }
    return rank;
}
