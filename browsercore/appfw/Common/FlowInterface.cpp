

#include "FlowInterface.h"
#include "qstmgestureevent.h"


namespace WRT {
GraphicsFlowInterface::GraphicsFlowInterface(QGraphicsItem* parent) : QGraphicsWidget(parent) 
{
    //grabGesture(QStm_Gesture::assignedType());
}


bool GraphicsFlowInterface::event(QEvent* event) 
{
    bool ret = false;     
    //ret = QStm_GestureEventFilter::instance()->event(event);
    if (!ret) {
        ret = QGraphicsWidget::event(event);
    }
    return ret;
}
}
