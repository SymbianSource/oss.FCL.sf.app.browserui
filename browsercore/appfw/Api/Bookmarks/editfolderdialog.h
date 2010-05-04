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


#ifndef EDITFOLDERDIALOG_H
#define EDITFOLDERDIALOG_H

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

class Ui_EditFolderDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *name;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(BaseDialog *EditFolderDialog)
    {
    if (EditFolderDialog->objectName().isEmpty())
        EditFolderDialog->setObjectName(QString::fromUtf8("EditFolderDialog"));

    verticalLayout = new QVBoxLayout(EditFolderDialog);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    label = new QLabel(EditFolderDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);

    verticalLayout->addWidget(label);

    name = new QLineEdit(EditFolderDialog);
    name->setObjectName(QString::fromUtf8("name"));

    verticalLayout->addWidget(name);

    verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer);

    buttonBox = new QDialogButtonBox(EditFolderDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);

    verticalLayout->addWidget(buttonBox);


    retranslateUi(EditFolderDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), EditFolderDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), EditFolderDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(EditFolderDialog);
    } // setupUi

    void retranslateUi(BaseDialog *EditFolderDialog)
    {
    EditFolderDialog->setWindowTitle(QApplication::translate("EditFolderDialog", "Edit Folder", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("EditFolderDialog", "Type a name for the folder.", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EditFolderDialog);
    } // retranslateUi

};

namespace Ui {
    class EditFolderDialog: public Ui_EditFolderDialog {};
} // namespace Ui

}

#endif // EDITFOLDERDIALOG_H
