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


#include "BaseDialog.h"

#define BASE_DIALOG_FLAGS ( Qt::Dialog )

static int dialogCounter = 0; 	
  
BaseDialog::BaseDialog(QWidget* parent, Qt::WindowFlags flags) :
	QDialog(parent, (BASE_DIALOG_FLAGS | flags))
{
	dialogCounter++; 
	if (parent)
	{
		setFixedWidth(parent->size().width() * .97);
	}
}

void BaseDialog::setWindowFlags(Qt::WindowFlags flags)
{
	QWidget::setWindowFlags(BASE_DIALOG_FLAGS | flags);
}

void BaseDialog::disableSizeGrip()
{
	QDialog::setSizeGripEnabled(false);
}

int BaseDialog::getCount()
{ 
    return dialogCounter; 
} 

BaseDialog::~BaseDialog()
{
    dialogCounter--;
}