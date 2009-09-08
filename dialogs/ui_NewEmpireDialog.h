/********************************************************************************
** Form generated from reading ui file 'NewEmpireDialog.ui'
**
** Created: Sat Sep 5 08:10:21 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_NEWEMPIREDIALOG_H
#define UI_NEWEMPIREDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewEmpireDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *empireNameEdit;
    QLabel *label_2;
    QLineEdit *empireIDEdit;
    QLabel *errorLabel;
    QHBoxLayout *horizontalLayout;
    QPushButton *acceptButton;
    QPushButton *rejectButton;

    void setupUi(QDialog *NewEmpireDialog)
    {
        if (NewEmpireDialog->objectName().isEmpty())
            NewEmpireDialog->setObjectName(QString::fromUtf8("NewEmpireDialog"));
        NewEmpireDialog->resize(258, 134);
        verticalLayout = new QVBoxLayout(NewEmpireDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_3 = new QLabel(NewEmpireDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMaximumSize(QSize(300, 20));
        QFont font;
        font.setPointSize(9);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setProperty("maximumSize", QVariant(QSize(300, 20)));
        label = new QLabel(NewEmpireDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        empireNameEdit = new QLineEdit(NewEmpireDialog);
        empireNameEdit->setObjectName(QString::fromUtf8("empireNameEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, empireNameEdit);

        label_2 = new QLabel(NewEmpireDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        empireIDEdit = new QLineEdit(NewEmpireDialog);
        empireIDEdit->setObjectName(QString::fromUtf8("empireIDEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, empireIDEdit);

        errorLabel = new QLabel(NewEmpireDialog);
        errorLabel->setObjectName(QString::fromUtf8("errorLabel"));

        formLayout->setWidget(2, QFormLayout::FieldRole, errorLabel);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        acceptButton = new QPushButton(NewEmpireDialog);
        acceptButton->setObjectName(QString::fromUtf8("acceptButton"));
        acceptButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout->addWidget(acceptButton);

        rejectButton = new QPushButton(NewEmpireDialog);
        rejectButton->setObjectName(QString::fromUtf8("rejectButton"));
        rejectButton->setMaximumSize(QSize(50, 16777215));

        horizontalLayout->addWidget(rejectButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(NewEmpireDialog);
        QObject::connect(rejectButton, SIGNAL(clicked()), NewEmpireDialog, SLOT(reject()));
        QObject::connect(acceptButton, SIGNAL(clicked()), NewEmpireDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(NewEmpireDialog);
    } // setupUi

    void retranslateUi(QDialog *NewEmpireDialog)
    {
        NewEmpireDialog->setWindowTitle(QApplication::translate("NewEmpireDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("NewEmpireDialog", "Create New Empire", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NewEmpireDialog", "Empire Name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("NewEmpireDialog", "Empire ID:", 0, QApplication::UnicodeUTF8));
        errorLabel->setText(QString());
        acceptButton->setText(QApplication::translate("NewEmpireDialog", "OK", 0, QApplication::UnicodeUTF8));
        rejectButton->setText(QApplication::translate("NewEmpireDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(NewEmpireDialog);
    } // retranslateUi

};

namespace Ui {
    class NewEmpireDialog: public Ui_NewEmpireDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWEMPIREDIALOG_H
