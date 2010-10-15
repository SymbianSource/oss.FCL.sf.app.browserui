#include "MostVisitedView.h"
#include "GridView.h"
#include "webpagecontroller.h"
#include "mostvisitedpagestore.h"
#include "wrtbrowsercontainer.h"
#include "ScaleThreePainter.h"
#include "qstmgestureevent.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneResizeEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsSimpleTextItem>

namespace GVA {


// -----------------------------------------------------------
// MostVisitedViewItem
// -----------------------------------------------------------

MostVisitedViewItem::MostVisitedViewItem(::MostVisitedPage *mostVisitedPage, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      m_mostVisitedPage(mostVisitedPage)
{
    grabGesture(QStm_Gesture::assignedType());
    installEventFilter(this);
}

void MostVisitedViewItem::activate() {
    WRT::WrtBrowserContainer * activePage = WebPageController::getSingleton()->currentPage();

    if (activePage) {
      activePage->mainFrame()->load(m_mostVisitedPage->m_url);
    }
    close();
    emit activated();
}

void MostVisitedViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(m_mostVisitedPage->m_pageThumbnail) {
        QPixmap pixmap(QPixmap::fromImage(*m_mostVisitedPage->m_pageThumbnail).scaled(size().toSize()));
        painter->drawPixmap(option->exposedRect, pixmap, option->exposedRect);
        
        QPen pen;
        int x, y, w, h;
        const int itemBorderWidth = 2;
        const QColor itemBorderColor = QColor("black");

        pen.setWidth(itemBorderWidth);
        pen.setBrush(itemBorderColor);
        pen.setJoinStyle(Qt::MiterJoin);
        
        painter->setPen(pen);
        option->exposedRect.toRect().getRect(&x, &y, &w, &h);
        painter->drawRect(x + itemBorderWidth / 2, y + itemBorderWidth / 2, w - itemBorderWidth, h - itemBorderWidth);
    }
}

bool MostVisitedViewItem::eventFilter(QObject* o, QEvent* e)
 { 
   if (o != this) 
      return false;
   return QStm_GestureEventFilter::instance()->eventFilter(o, e);
 } 

bool MostVisitedViewItem::event(QEvent* event)
 { 
   if (event->type() == QEvent::Gesture) {
       QStm_Gesture* gesture = getQStmGesture(event);
       if (gesture) {  
           handleQStmGesture(gesture);
           event->accept();
           return true;
       }
   }
   return false;
 }

bool MostVisitedViewItem::handleQStmGesture(QStm_Gesture* gesture)
 {
   QStm_GestureType type = gesture->getGestureStmType();
   
   if (type == QStmTapGestureType) {
       activate();
   }
   return true;
 }


// -----------------------------------------------------------
// MostVisitedView
// -----------------------------------------------------------

MostVisitedView::MostVisitedView(const QString &title, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      m_layout(new QGraphicsLinearLayout(Qt::Vertical, this)),
      m_title(0),
      m_gridView(new GridView(this)),
      m_backgroundPixmap(0),
      m_backgroundDirty(true),
      m_titleWrapper(new QGraphicsWidget(this))
      
{
    m_titleWrapper->resize(50, 10);
    m_title = new QGraphicsSimpleTextItem("  " + title, m_titleWrapper);
    m_title->setBrush(QColor(Qt::white));
    QFont textFont = m_title->font();
    #ifdef Q_WS_MAEMO_5
    textFont.setPointSize(16);
    #else
    textFont.setPointSize(6);
    #endif
    textFont.setWeight(QFont::Bold);
    m_title->setFont(textFont);
    #ifdef Q_WS_MAEMO_5
    m_titleWrapper->setContentsMargins(16, 0, 0, 0);
    #endif

    setLayout(m_layout);
    m_layout->setSpacing(0);
    
    #ifdef Q_WS_MAEMO_5
    m_layout->setContentsMargins(16, 4, 16, 16);    
    m_layout->addItem(m_titleWrapper);
    #else
    
    m_layout->addItem(m_titleWrapper); 
    #endif
    m_layout->addItem(m_gridView);

		#ifdef Q_WS_MAEMO_5
    m_gridView->setColumnRowCounts(3, 2);    
    #endif

    m_backgroundPainter = new ScaleThreePainter(
            ":/mostvisited/most_visited_bac_left.png",
            ":/mostvisited/most_visited_bac_middle.png",
            ":/mostvisited/most_visited_bac_right.png"
            );   
}

MostVisitedView::~MostVisitedView() {
    delete m_backgroundPainter;
    delete m_backgroundPixmap;
}

void MostVisitedView::update(QString mode) {
	  
    MostVisitedPageList pageList = MostVisitedPageStoreSingleton::Instance().pageList();
    m_gridView->clear();
    foreach(MostVisitedPage *page, pageList) {
        MostVisitedViewItem *item = new MostVisitedViewItem(page, this);
        //qDebug() << __PRETTY_FUNCTION__ << " url=" << page->m_url << page->m_pageThumbnail;
        m_gridView->addItem(item);
        safe_connect(item, SIGNAL(activated()), this, SLOT(onItemActivated()));
    }
    
    m_displayMode = mode;       
    m_gridView->rebuildLayout();
}

void MostVisitedView::resizeEvent(QGraphicsSceneResizeEvent * event) {
	  
    m_gridView->resize(event->newSize());  
    // Force background update on next paint().
    m_backgroundDirty = true;
}

void MostVisitedView::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    emit closeComplete();
}

void MostVisitedView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // Update the background pixmap if needed.
    
    #ifndef Q_WS_MAEMO_5
    if (m_displayMode == "portrait")
    {
    	m_gridView->setColumnRowCounts(2, 3);
    	m_titleWrapper->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    	m_layout->setContentsMargins(16, 4, 16, 16);  
    	setMinimumSize(346, 523);    
    	setMaximumSize(346, 523);    
    }
    else
    {
    	m_gridView->setColumnRowCounts(3, 2);
    	m_titleWrapper->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    	m_layout->setContentsMargins(36, 4, 0, 0);   
    	setMinimumSize(610, 260);
    	setMaximumSize(610, 260);
    }
    m_gridView->rebuildLayout();
    #endif
    
    #ifndef Q_WS_MAEMO_5   
    updateBackgroundPixmap(geometry().size().toSize(), widget);    	
    m_backgroundPainter->unloadPixmaps();
    #else
    if(m_backgroundDirty) {    	
     	updateBackgroundPixmap(geometry().size().toSize(), widget);    	
      m_backgroundPainter->unloadPixmaps();
    }
    #endif

    // Paint the background.
    painter->drawPixmap(option->exposedRect, *m_backgroundPixmap, option->exposedRect);
}

void MostVisitedView::updateBackgroundPixmap(const QSize &size, QWidget* widget) {
    delete m_backgroundPixmap;
    m_backgroundPixmap = new QPixmap(size);
    m_backgroundPixmap->fill(QColor(0xff, 0xff, 0xff, 0));
    QPainter painter(m_backgroundPixmap);
    m_backgroundPainter->paint(&painter, QRect(0, 0, size.width(), size.height()), widget);
    m_backgroundDirty = false;
}

void MostVisitedView::onItemActivated() {
    close();
    emit activated();
}

}  // namespace

