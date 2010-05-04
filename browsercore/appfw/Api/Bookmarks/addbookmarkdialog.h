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


#ifndef ADDBOOKMARKDIALOG_H
#define ADDBOOKMARKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "BaseDialog.h"

namespace WRT {

class Ui_AddBookmarkDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *name;
    QLineEdit *address;
    QComboBox *location;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(BaseDialog *AddBookmarkDialog)
    {
    if (AddBookmarkDialog->objectName().isEmpty())
        AddBookmarkDialog->setObjectName(QString::fromUtf8("AddBookmarkDialog"));

    verticalLayout = new QVBoxLayout(AddBookmarkDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    label = new QLabel(AddBookmarkDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);

    verticalLayout->addWidget(label);

    name = new QLineEdit(AddBookmarkDialog);
    name->setObjectName(QString::fromUtf8("name"));

    verticalLayout->addWidget(name);

    address = new QLineEdit(AddBookmarkDialog);
    address->setObjectName(QString::fromUtf8("address"));

    verticalLayout->addWidget(address);

    location = new QComboBox(AddBookmarkDialog);
    location->setObjectName(QString::fromUtf8("location"));

    verticalLayout->addWidget(location);

    verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(AddBookmarkDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);

    verticalLayout->addWidget(buttonBox);


    retranslateUi(AddBookmarkDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), AddBookmarkDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), AddBookmarkDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(AddBookmarkDialog);
    } // setupUi

    void retranslateUi(BaseDialog *AddBookmarkDialog)
    {
    AddBookmarkDialog->setWindowTitle(QApplication::translate("AddBookmarkDialog", "Add Bookmark", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("AddBookmarkDialog", "Type a name for the bookmark, and choose where to keep it.", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(AddBookmarkDialog);
    } // retranslateUi

};

namespace Ui {
    class AddBookmarkDialog: public Ui_AddBookmarkDialog {};
} // namespace Ui

}

#endif // ADDBOOKMARKDIALOG_H
