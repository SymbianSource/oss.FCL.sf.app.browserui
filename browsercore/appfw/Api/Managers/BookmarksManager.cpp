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



#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtGui>
#include <QtCore/QDebug>
#include <QtCore/QUrl>

#include <QString>
#include "BookmarksManager_p.h"
#include "BookmarksManager.h"

//#include "wrtsettings.h"
#include "bedrockprovisioning.h"

#include "bookmarks.h"
#include "xbel.h"
#include <browsercontentdll.h>
#include "webpagecontroller.h"
#include "wrtbrowsercontainer.h"


namespace WRT {

#define SETTINGMAXURLS "MaxRecentUrls"

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
}

BookmarksManagerPrivate::~BookmarksManagerPrivate()
{
    delete m_bookmarkRootNode;
    delete m_historyRootNode;
	delete m_historyRootNodeProxy;
    delete m_bookmarkSession;
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
}
BookmarksManager::~BookmarksManager()
{
    delete d;
}


/* overwrite settings. called at construction and when settings change
 */
//void BookmarksManager::setSettings(WrtSettings *settings)
void BookmarksManager::setSettings(BEDROCK_PROVISIONING::BedrockProvisioning *settings)	
{
    if(settings)   {
       d->m_maxUrls = settings->valueAsInt(SETTINGMAXURLS);
    }
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
            addBookmark(node->url,node->title,0);
        
    } 
#endif    
}

QString BookmarksManager::getBookmarksJSON()
{
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->FetchAllBookmarks();
    QString bookmakrData = "[";
    for(int i=0;i<nodes.count();i++) {
        bookmakrData.append("{");
        bookmakrData.append("\"title\": \"");
        bookmakrData.append(nodes[i]->getTitle());
        bookmakrData.append("\", \"urlvalue\": \"");
        bookmakrData.append(nodes[i]->getUrl());
            if(i != (nodes.count()-1))
            bookmakrData.append("\"},");
        else
            bookmakrData.append("\"}");
    }
    bookmakrData.append("]");

    qDeleteAll(nodes);
	nodes.clear();
 
	return bookmakrData;

}

int BookmarksManager::addBookmark(const QString &url, const QString &title, int index)
{
    QString updatedTitle = title;
    //Setting title as url string if title is not available
    if(url.isEmpty())
        return ErrBookmarkUrlEmpty;
    
    QList<BookmarkLeaf*> nodes;
    nodes = d->m_bookmarkSession->FetchAllBookmarks();
    for (int iter = 0 ; iter < nodes.count() ; ++iter)   {
     if(nodes.at(iter)->getTitle() == title)   {
        //Node already exists no need to add, just return
        return ErrBookmarkAllReadyPresent;
      }
    }
	
    qDeleteAll(nodes);
	nodes.clear();
    
    //Setting title as url string if title is not available
    if(title.isEmpty())
        updatedTitle = url;

    BookmarkLeaf* leaf=new BookmarkLeaf();
    leaf->setTitle(updatedTitle);
    leaf->setUrl(url);
    //leaf->setDate(bookmark->date);
    leaf->setTag("unknown");
    leaf->setIndex(index);
    
    if(ErrNone == d->m_bookmarkSession->AddBookmark(leaf)) {
		delete leaf;
        return ErrNone;
    }
	delete leaf;
    return ErrGeneral;
}

void BookmarksManager::deleteBookmark(QString title)
{
    d->m_bookmarkSession->DeleteBookmark(title);
}

int BookmarksManager::reorderBokmarks(QString title,int new_index)
{
    d->m_bookmarkSession->reorderBokmarks(title,new_index);
    return ErrNone;    
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

QString BookmarksManager::getHistoryFoldersJSON()
{
    loadHistory();
    loadHistoryProxy();

    QString historyFolders = "[";

    QList<BookmarkNode *> rootChildren (d->m_historyRootNodeProxy->children());

    for (int i=0; i < rootChildren.count(); i++) {
        //check for folder nodes
        if (rootChildren.at(i)->type() == BookmarkNode::Folder) {
            historyFolders.append("\"");
            historyFolders.append (rootChildren.at(i)->title );
            historyFolders.append("\"");
            
            if(i != rootChildren.count()-1)
               historyFolders.append ( ",");
        }
        
    }
    historyFolders.append ("]");

    return historyFolders;
}

QString BookmarksManager::getHistoryFolderJSON(QString folderName)
{
    QString history = "";

    QList<BookmarkNode *> rootChildren (d->m_historyRootNodeProxy->children());

    for (int i=0; i < rootChildren.count(); i++) {
        //check for folder nodes
        if ((rootChildren.at(i)->type() == BookmarkNode::Folder) && (rootChildren.at(i)->getTitle() == folderName)) {

            QList<BookmarkNode *> nodeChildren (rootChildren.at(i)->children());
            history.append("[");
    
            for (int j=0; j < nodeChildren.count(); j++) {
                //If you encounter a folder node, jump to next sibling since 
                //there cann't by any URL at this level
                if(nodeChildren.at(j)->type() == BookmarkNode::Folder) {
                    QList<BookmarkNode *> leafChildren (nodeChildren.at(j)->children());
                    for (int k=0; k < leafChildren.count(); k++) {
                        QString title,date,time;
                        history.append("{");
                         history.append("\"titleVal\": \"");
                         title = leafChildren.at(k)->getTitle();
                         history.append(title);
                         history.append("\", \"dateVal\": \"");
                         date = nodeChildren.at(j)->getDate().toString("dd.MM.yyyy");
                         history.append(date);
                         history.append("\", \"urlVal\": \"");
                         history.append(leafChildren.at(k)->getUrl());

                         history.append("\", \"timeVal\": \"");
                         time =leafChildren.at(k)->getLastVisited().toString("h:mm ap");
                         history.append(time);

//                       qDebug() <<" Title : "<<title << " Date : "<< date <<" Time : "<<time<<'\n';
                    
                        if(j != (nodeChildren.count()-1) || k != (leafChildren.count()-1)  )
                           history.append("\"},");
                        else
                           history.append("\"}");
                        } 
                    }
                else{
                    history.append("{");
                    history.append("\"titleVal\": \"");
                    history.append(nodeChildren.at(j)->getTitle());
                    history.append("\", \"dateVal\": \"");
                    history.append(rootChildren.at(i)->getDate().toString("dd.MM.yyyy"));
                    history.append("\", \"urlVal\": \"");
                    history.append(nodeChildren.at(j)->getUrl());
                    history.append("\", \"timeVal\": \"");
                    QString time =nodeChildren.at(j)->getLastVisited().toString("h:mm ap");
                    history.append(time);
                
                    if(j != (nodeChildren.count()-1))
                       history.append("\"},");
                    else
                       history.append("\"}");
                    } 
            }//EOF for (int j=0; j < nodeChildren.count(); j++) 

        }//EOF IF if (rootChildren.at(i)->type() == BookmarkNode::Folder && rootChildren.at(i)->title() == folder) {
        else {
            //at this level there cann't be URL items.
        }

        
    }
    history.append ("]");

    return history;
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
    
    QDateTime currentDateTime = QDateTime::currentDateTime();
    
    HistoryLeaf* leaf=new HistoryLeaf();
    leaf->setTitle(title);
    leaf->setUrl(url);
    leaf->setDate(currentDateTime.date());
    leaf->setLastVisited(currentDateTime.time());
    
    if (d->m_connectedToBookmarks){
        if(ErrNone == d->m_bookmarkSession->AddHistory(leaf)){
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
