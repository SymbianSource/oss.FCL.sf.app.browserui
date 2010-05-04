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


#ifndef LoadController_h
#define LoadController_h

#include <QWidget>
#include <QAction>
#include <QNetworkReply>
#include "BWFGlobal.h"


namespace WRT {

    class BWF_EXPORT LoadController : public QObject
    {
        Q_OBJECT
    public:
        enum GotoBrowserMode {
            GotoModeLoading, /**< Loading Mode (i.e. page is loading) */
            GotoModeEditing, /**< Editing Mode (i.e. user can or is editing the url) */
            GotoModeReloadable, /**< Reloadable Mode (i.e. the url has not changed, and can be reloaded) */
            GotoModeInvalid
        };

       /*!
        * LoadController Constructor
        * @param parent: parent QObject
        */
        LoadController();
        /*!
         * LoadController Destructor
         */
        ~LoadController();


    public:
        GotoBrowserMode mode();
        void setMode(GotoBrowserMode mode);
        

		
        bool loadCanceled() { return m_canceled ;}
        void setCanceled(bool val) {m_canceled = val;}
        
        void setUrlText(QString );
        QString urlText() {return m_textBoxValue; }
        
        int progressValue() {return m_progress; }
        bool inline isPageLoading(){ return m_isPageLoading;}


   public slots:
        void loadStarted();
        void loadProgress( const int progress );
        void loadFinished( const bool ok );
        void urlChanged(QUrl);
 

Q_SIGNALS:
        void pageLoadStarted();
        void pageLoadProgress(const int);
        void pageLoadFinished(const bool ok);
        void pageUrlChanged(QString);

    private:
        GotoBrowserMode m_gotoMode;
        bool m_isPageLoading;
        bool m_initialLayoutIsComplete;
        bool m_canceled;
        int  m_progress;
        QString m_textBoxValue;


    };
}
#endif // __LOADCONTROLLER_H__
