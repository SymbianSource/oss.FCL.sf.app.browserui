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


#ifndef EDITBOOKMARKDIALOG_H
#define EDITBOOKMARKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "BaseDialog.h"

namespace WRT {

class Ui_EditBookmarkDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *name;
    QLineEdit *address;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(BaseDialog *EditBookmarkDialog)
    {
    if (EditBookmarkDialog->objectName().isEmpty())
        EditBookmarkDialog->setObjectName(QString::fromUtf8("EditBookmarkDialog"));

    verticalLayout = new QVBoxLayout(EditBookmarkDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    label = new QLabel(EditBookmarkDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);

    verticalLayout->addWidget(label);

    name = new QLineEdit(EditBookmarkDialog);
    name->setObjectName(QString::fromUtf8("name"));

    verticalLayout->addWidget(name);

    address = new QLineEdit(EditBookmarkDialog);
    address->setObjectName(QString::fromUtf8("address"));

    verticalLayout->addWidget(address);

    verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(EditBookmarkDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);

    verticalLayout->addWidget(buttonBox);


    retranslateUi(EditBookmarkDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), EditBookmarkDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), EditBookmarkDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(EditBookmarkDialog);
    } // setupUi

    void retranslateUi(BaseDialog *EditBookmarkDialog)
    {
    EditBookmarkDialog->setWindowTitle(QApplication::translate("EditBookmarkDialog", "Edit Bookmark", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("EditBookmarkDialog", "Type a name and url for the bookmark.", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EditBookmarkDialog);
    } // retranslateUi

};

namespace Ui {
    class EditBookmarkDialog: public Ui_EditBookmarkDialog {};
} // namespace Ui

}

#endif // EDITBOOKMARKDIALOG_H
