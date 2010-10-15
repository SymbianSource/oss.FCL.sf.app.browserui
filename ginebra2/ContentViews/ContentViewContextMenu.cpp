
#include <QDebug>
#include <QWebHitTestResult>

#include "ContentViewContextMenu.h"
#include "webpagecontroller.h"
#include "Downloads.h"

namespace GVA {

ContentViewContextMenu::ContentViewContextMenu(QWebHitTestResult *hitTest, QWidget *parent) :
    ContextMenu(parent),
    m_menu(parent),
    m_hitTest(hitTest)
{
    if(m_hitTest->linkUrl().isValid()) {
        addAction("Open in New Window", this, SLOT(onOpenLink()));
        addAction("Share Link", this, SLOT(onShareLink()));
    }
    if(m_hitTest->imageUrl().isValid()) {
        addAction("View Image", this, SLOT(onViewImage()));
        addAction("Save Image", this, SLOT(onSaveImage()));
        addAction("Share Image", this, SLOT(onShareImage()));

    }
}

void ContentViewContextMenu::onOpenLink() {
    WebPageController::getSingleton()->LoadInNewWindow(m_hitTest->linkUrl().toString());
}

void ContentViewContextMenu::onViewImage() {
    WebPageController::getSingleton()->LoadInNewWindow(m_hitTest->imageUrl().toString());
}

void ContentViewContextMenu::onShareImage() {
    WebPageController::getSingleton()->share(m_hitTest->imageUrl().toString());
}

void ContentViewContextMenu::onSaveImage() {
}

void ContentViewContextMenu::onShareLink() {
    WebPageController::getSingleton()->share(m_hitTest->linkUrl().toString());

}

}  // GVA namespace
