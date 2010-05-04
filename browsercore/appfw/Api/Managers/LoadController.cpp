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



#include "LoadController.h"
#include <QDebug>

namespace WRT {

LoadController::LoadController ( ) :
    m_gotoMode(GotoModeEditing), 
    m_isPageLoading(false), 
    m_canceled(false),
    m_progress(0)
{

}

LoadController::~LoadController()
{

}

void LoadController::setMode(GotoBrowserMode mode)
{
    m_gotoMode = mode;
}

LoadController::GotoBrowserMode LoadController::mode()
{
    return m_gotoMode;
}

void LoadController::loadStarted()
{
    m_progress = 0;
    m_canceled = 0;
    m_isPageLoading = true;

    m_gotoMode = GotoModeLoading;

    //qDebug() << __PRETTY_FUNCTION__;
    emit pageLoadStarted();
}

void LoadController::loadFinished(bool ok)
{
    //qDebug() << __PRETTY_FUNCTION__  << ok << m_gotoMode ;
    m_isPageLoading = false;

    m_progress = 100;

    m_gotoMode = GotoModeReloadable;

// TODO: Change to editing mode if load failed
/*
    // if page succeed, set the text and goto reloading mode, else load mode
    if(ok)
    {
        m_gotoMode = GotoModeReloadable;

    }
    else
    {
        m_gotoMode = GotoModeEditing;
    }
*/
    emit pageLoadFinished(ok);
    //qDebug() << __PRETTY_FUNCTION__  << m_gotoMode ;
}

void LoadController::loadProgress(int progress)
{
    m_progress = progress;

    //qDebug() << __PRETTY_FUNCTION__  << progress ;
    emit pageLoadProgress(progress);
}

void LoadController::urlChanged(QUrl url) 
{
//    qDebug() << __PRETTY_FUNCTION__  << url;
    // Save the url 
    m_textBoxValue = url.toString();
    emit pageUrlChanged(m_textBoxValue);
}
	
void LoadController::setUrlText(QString str)
{
//    qDebug() << __PRETTY_FUNCTION__  << str;
    m_textBoxValue = str; 
}

} // namespace WRT
