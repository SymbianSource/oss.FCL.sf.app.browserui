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


#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>

class BaseDialog : public QDialog
{
	Q_OBJECT

public:
	BaseDialog(QWidget* parent = NULL, Qt::WindowFlags flags = 0);
	void setWindowFlags(Qt::WindowFlags flags);
	void disableSizeGrip();
    static int getCount(); // get the dialog count
	~BaseDialog();   // destructor 
	
};

#endif // BASEDIALOG_H
