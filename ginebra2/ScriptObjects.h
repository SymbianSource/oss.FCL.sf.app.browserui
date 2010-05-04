/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SCRIPTOBJECTS_H
#define SCRIPTOBJECTS_H

#include <QObject>
#include <QRect>

// -------------------------------------------------------

/*! \ingroup JavascriptAPI
  Wrapper for QRect that can be passed to javascript.
*/
class ScriptRect : public QObject, public QRect {
    Q_OBJECT
  public:
    ScriptRect(const QRect &rect) : QRect(rect) { setObjectName("scriptrect"); }

    int getx() const { return x(); }
    Q_PROPERTY(int x READ getx)
    int gety() const { return y(); }
    Q_PROPERTY(int y READ gety)
    int getwidth() const { return width(); }
    Q_PROPERTY(int width READ getwidth)
    int getheight() const { return height(); }
    Q_PROPERTY(int height READ getheight)
};

// -------------------------------------------------------

/*! \ingroup JavascriptAPI
  Wrapper for QRectF that can be passed to javascript.
*/
class ScriptRectF : public QObject, public QRectF {
    Q_OBJECT
  public:
    ScriptRectF(const QRectF &rect) : QRectF(rect) { setObjectName("scriptrectf"); }

    qreal getx() const { return x(); }
    Q_PROPERTY(qreal x READ getx)
    qreal gety() const { return y(); }
    Q_PROPERTY(qreal y READ gety)
    qreal getwidth() const { return width(); }
    Q_PROPERTY(qreal width READ getwidth)
    qreal getheight() const { return height(); }
    Q_PROPERTY(qreal height READ getheight)
};

// -------------------------------------------------------

/*! \ingroup JavascriptAPI
  Wrapper for QPoint that can be passed to javascript.
*/
class ScriptPoint : public QObject, public QPoint {
    Q_OBJECT
  public:
    ScriptPoint() { setObjectName("scriptpoint"); }
    ScriptPoint(QObject *parent, const QPoint &p, const QString &objName = QString::null) 
      : QObject(parent), QPoint(p) 
      { setObjectName(objName.isNull() ? "scriptpoint" : objName); }
    ScriptPoint(const QPoint &p) : QPoint(p) { setObjectName("scriptpoint"); }

    int getx() const { return x(); }
    Q_PROPERTY(int x READ getx)
    int gety() const { return y(); }
    Q_PROPERTY(int y READ gety)
};

// -------------------------------------------------------

/*! \ingroup JavascriptAPI
  Wrapper for QPointF that can be passed to javascript.
*/
class ScriptPointF : public QObject, public QPointF {
    Q_OBJECT
  public:
    ScriptPointF(const QPointF &p) : QPointF(p) { setObjectName("scriptrect"); }

    qreal getx() const { return x(); }
    Q_PROPERTY(qreal x READ getx)
    qreal gety() const { return y(); }
    Q_PROPERTY(qreal y READ gety)
};

// -------------------------------------------------------

/*! \ingroup JavascriptAPI
  Wrapper for QSize that can be passed to javascript.
*/
class ScriptSize : public QObject, public QSize {
    Q_OBJECT
  public:
    ScriptSize(const QSize &size) : QSize(size) { setObjectName("scriptsize"); }

    int getwidth() const { return width(); }
    Q_PROPERTY(int width READ getwidth)
    int getheight() const { return height(); }
    Q_PROPERTY(int height READ getheight)
};

#endif // SCRIPTOBJECTS_H
