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


#ifndef __WEBPAGEDATA_H__
#define __WEBPAGEDATA_H__

#include <QImage>
#include <QPoint>
#include <QMetaType>

class WebPageData : public QObject
{
    Q_OBJECT
public:
    WebPageData(){};
    WebPageData(QObject *parent) : QObject(parent) {};
    WebPageData(const WebPageData& o) :  m_thumbnail(o.m_thumbnail),m_zoomFactor(o.m_zoomFactor),m_contentsPos(o.m_contentsPos) {};
    QImage  m_thumbnail;
    qreal   m_zoomFactor;
    QPoint  m_contentsPos;
};
Q_DECLARE_METATYPE (WebPageData)
QDataStream &operator<<(QDataStream &out, const WebPageData &myObj);
QDataStream &operator>>(QDataStream &in, WebPageData &myObj);
#endif
