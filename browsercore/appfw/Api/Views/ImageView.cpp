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


#include <QStack>
#include <qpainter.h>
#include "WrtPageManager.h"
#include "FlowInterface.h"
#include "ImageView_p.h"
#include "ImageView.h"

#include "qwebhistory.h"
#include "qwebframe.h"
#include "qwebelement.h"
#include "wrtbrowsercontainer.h"
#include "webpagedata.h"

#include <QDebug>


#define NO_IMAGE -1
#define IMAGE_ALT "alt"
#define IMAGE_SOURCE "src"

namespace WRT {

ImageViewPrivate::ImageViewPrivate(WrtPageManager * mgr, QWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(parent),
    m_graphicsWidgetParent(0),
    m_pageManager(mgr),
    m_imageIndex(0),
    m_activePage(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);
    init();
}

ImageViewPrivate::ImageViewPrivate(WrtPageManager * mgr, QGraphicsWidget* parent) :
    m_flowInterface(0),
    m_widgetParent(0),
    m_graphicsWidgetParent(parent),
    m_pageManager(mgr),
    m_imageIndex(0),
    m_activePage(0),
    m_isActive(false)
{
    Q_ASSERT(m_pageManager);
    init();
}

ImageViewPrivate::~ImageViewPrivate()
{
}

void ImageViewPrivate::init()
{
    // create the view's actions
    m_actionForward = new QAction("Forward",m_widgetParent);
    m_actionForward->setObjectName("Forward");
    m_actionBack = new QAction("Back",m_widgetParent);
    m_actionBack->setObjectName("Back");
    m_actionOK = new QAction("OK",m_widgetParent);
    m_actionOK->setObjectName("OK");
    m_actionCancel = new QAction("Cancel",m_widgetParent);
    m_actionCancel->setObjectName("OK");
}

/*!
 * \class ImageView
 *
 * \brief The base class for the ImageViews
 *
 * This class is responsible for....
 */

/*!
  Basic ImageView constructor requires a PageManager to manage the pages
  and a parent QWidget
*/
ImageView::ImageView(WrtPageManager * pageMgr, QWidget* parent) :
    d(new ImageViewPrivate(pageMgr, parent))
{
    // always trigger an update to track the images regardless of whether active or not
    connect(d->m_pageManager, SIGNAL(loadFinished(bool)), this, SLOT(updateImageList(bool)));
}

/*!
  Basic ImageView constructor requires a PageManager to manage the pages
  and a parent QGraphicsWidget
*/
ImageView::ImageView(WrtPageManager * pageMgr, QGraphicsWidget* parent) :
    d(new ImageViewPrivate(pageMgr, parent))
{
    // always trigger an update to track the images regardless of whether active or not
    connect(d->m_pageManager, SIGNAL(loadFinished(bool)), this, SLOT(updateImageList(bool)));
}


ImageView::~ImageView()
{
    delete d;
}

/*!
  Retrieve the WrtPageManager assigned to this view
*/
WrtPageManager* ImageView::wrtPageManager()
{
    return d->m_pageManager;
}

/*!
  Update image list. This is automatically called when a page loads
  regardless of if the view is active or not.
*/
void ImageView::updateImageList(bool ok)
{
    d->m_activePage = d->m_pageManager->currentPage();

    if(!d->m_activePage)
        return;        

    // reset image list
    d->m_imageList.clear();

    // walk the tree of images to fetch all available images on a page.
    QStack<QWebFrame*> stackOfFrames;
    stackOfFrames.push(d->m_activePage->mainFrame());
    while( !stackOfFrames.isEmpty()) 
      {
        QWebFrame* f = stackOfFrames.pop();

        foreach( QWebElement element, 
                 f->findAllElements( "img" ))
          {
             bool found = false;
             QRect r = element.geometry();
             if( r.size().isEmpty())
                {
                  continue;
                }

             QString imgsrc = element.attribute( IMAGE_SOURCE );

             if( imgsrc.isEmpty())
               found = true; // do not add empty image in any case
             else
               {
                 foreach( QImage c_img,d->m_imageList )
                   {
                     if( imgsrc == c_img.text( IMAGE_SOURCE ))
                       {
                         found = true;
                         break;
                       }
                   }
    
               }

             if( !found )
               {
                 QImage img( element.geometry().size(),QImage::Format_RGB32 );
                 QPainter p( static_cast<QPaintDevice *>(&img) );
#if defined CWRTINTERNALWEBKIT
                 f->renderElement( &p,element,QPoint( 0,0 ));
// fixme. implementation is in QtWebKit ToT, not yet in QtWebKit 4.6 beta				 
// #elif QT_VERSION >= 0x040600
                 // element.render(&p);		
#endif
                 img.setText( IMAGE_SOURCE,imgsrc );
                 img.setText( IMAGE_ALT,element.attribute( IMAGE_ALT ));
                 d->m_imageList.append( img );
               }
          }

        foreach (QWebFrame *child, f->childFrames())
            stackOfFrames.push(child);
      }

    // IF we're active, update the visible image list
    if(d->m_flowInterface && d->m_isActive)
        refreshFlowInterface();
}


/*!
  Return the widget handle of this view
*/
QWidget* ImageView::qWidget() const
{
    return d->m_flowInterface;
}

/*!
  Return the title of this view for display
*/
QString ImageView::title()
{
    return QString("ImageView");
}

/*!
  Return whether this view is active or not
*/
bool ImageView::isActive()
{
    return d->m_isActive;
}


/*!
  Return the list of public QActions most relevant to the view's current context
  (most approptiate for contextual menus, etc.
*/
QList<QAction*> ImageView::getContext()
{
    // for now, all actions valid at all times
    // but there may be some logic here to determine context
    QList<QAction*> contextList;
    contextList << 
        d->m_actionForward <<
        d->m_actionBack <<
        d->m_actionOK <<
        d->m_actionCancel;
    return contextList;
}

/*!
  Get the current image from the flow interface
*/
QImage ImageView::getCurrent() const
{
    int index = d->m_flowInterface->centerIndex();
    QImage img(d->m_flowInterface->slide(index));
    if(!img.isNull())
        return img;
    return QImage();
}

/*!
  Get the current image count (valid even if view is not active)
*/
int ImageView::getCurrentImageCount()
{
    return d->m_imageList.count();
}

/*!
  Return the view's Forward QAction
  For scrolling the history view forwards
*/
QAction * ImageView::getActionForward()
{
    return d->m_actionForward;
}

/*!
  Return the view's Back QAction
  For scrolling the history view backwards
*/
QAction * ImageView::getActionBack()
{
    return d->m_actionBack;
}

/*!
  Return the view's OK QAction
  For invoking the view's OK
*/
QAction * ImageView::getActionOK()
{
    return d->m_actionOK;
}

/*!
  Return the view's Cancel QAction
  For invoking the view's Cancel
*/
QAction * ImageView::getActionCancel()
{
    return d->m_actionCancel;
}

void ImageView::refreshFlowInterface()
{
    if(!d->m_flowInterface)
        return;

    // clear
    d->m_flowInterface->clear();
    if (d->m_imageList.count() > 0) {
        for (int i=0; i < d->m_imageList.count(); i++)
            d->m_flowInterface->addSlide(d->m_imageList.at(i));
 
        d->m_flowInterface->setCenterIndex(0);
        centerIndexChanged(0);
    }
    updateActions(0);
}

/*!
  activate the view's resources. Could be connected by client to view visibility
*/
void ImageView::activate()
{
    Q_ASSERT(!d->m_isActive);

    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->init();
    
    // now forward visual flow lite's signals
    connect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), SIGNAL(centerIndexChanged(int)));
    connect(d->m_flowInterface, SIGNAL(ok(int)), this, SLOT(chooseImage(int)));
    connect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));

    // internally process the index change signal as well
    connect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(updateActions(int)));

    // auto-link relevant actions to slots
    connect(d->m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    connect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));

    // show and set index
    d->m_imageIndex = 0;
    d->m_isActive = true;

    // finally update the images at the end (will also update the interface) just before showing
    updateImageList(true);
    d->m_flowInterface->show();
}
    
/*!
  deactivate the view's resources. Could be connected by client to view visibility
*/
void ImageView::deactivate()
{
    Q_ASSERT(d->m_isActive);

    if(!d->m_flowInterface)
        return;

    // disconnect signals
    disconnect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SIGNAL(centerIndexChanged(int)));
    disconnect(d->m_flowInterface, SIGNAL(ok(int)), this, SLOT(chooseImage(int)));
    disconnect(d->m_flowInterface, SIGNAL(cancel()), this, SIGNAL(cancel()));
    
    // internally process the index change signal as well
    disconnect(d->m_flowInterface, SIGNAL(centerIndexChanged(int)), this, SLOT(updateActions(int)));

    // auto-link relevant actions to slots
    disconnect(d->m_actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    disconnect(d->m_actionBack, SIGNAL(triggered()), this, SLOT(back()));

    // cleanup
    d->m_flowInterface->deleteLater();
    d->m_flowInterface = NULL;
    d->m_isActive = false;
}
    
/*!
  set the history view's center index
*/
void ImageView::setCenterIndex(int index)
{
    // first emit center index change
    //emit centerIndexChanged(index);
    
    d->m_imageIndex = index - d->m_activePage->history()->currentItemIndex();
    QWebHistoryItem item = d->m_activePage->history()->itemAt(d->m_imageIndex);
    QString title;
    if (item.isValid()) {
        title = item.title();
        if (title.isNull() || title.isEmpty()) {
            title = item.url().toString();
        }
    }
    emit titleChanged(title);
    emit centerIndexChanged(index);
}

/*!
  scroll forward in the view
*/
void ImageView::forward()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showNext();
}

/*!
  scroll back in the view
*/
void ImageView::back()
{
    if(!d->m_flowInterface)
        return;

    d->m_flowInterface->showPrevious();
}

void ImageView::updateActions(int centerIndex)
{
    d->m_actionBack->setEnabled(centerIndex != 0);
    d->m_actionForward->setEnabled(centerIndex < (d->m_activePage->history()->count() - 1));
}



void ImageView::chooseImage(int index)
{
    QImage img(d->m_flowInterface->slide(index));
    if(!img.isNull())
        emit ok(img);
}

/*!
  \fn void ImageView::centerIndexChanged(int index);
  emitted when the center index changed
*/

/*!
  \fn void ImageView::ok(QImage img);
  emitted when the ok action has occured, contains history index at the time of close
*/

/*!
  \fn void ImageView::cancel();
  emitted when the cancel action has occured
*/

} // namespace WRT
