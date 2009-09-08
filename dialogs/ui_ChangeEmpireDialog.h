/********************************************************************************
** Form generated from reading ui file 'ChangeEmpireDialog.ui'
**
** Created: Tue Sep 8 07:57:24 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CHANGEEMPIREDIALOG_H
#define UI_CHANGEEMPIREDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChangeEmpireDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_5;
    QComboBox *empireCombo_3;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *ChangeEmpireDialog)
    {
        if (ChangeEmpireDialog->objectName().isEmpty())
            ChangeEmpireDialog->setObjectName(QString::fromUtf8("ChangeEmpireDialog"));
        ChangeEmpireDialog->resize(238, 88);
        verticalLayout = new QVBoxLayout(ChangeEmpireDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_5 = new QLabel(ChangeEmpireDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_5);

        empireCombo_3 = new QComboBox(ChangeEmpireDialog);
        empireCombo_3->setObjectName(QString::fromUtf8("empireCombo_3"));

        verticalLayout->addWidget(empireCombo_3);

        buttonBox = new QDialogButtonBox(ChangeEmpireDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ChangeEmpireDialog);

        QMetaObject::connectSlotsByName(ChangeEmpireDialog);
    } // setupUi

    void retranslateUi(QWidget *ChangeEmpireDialog)
    {
        ChangeEmpireDialog->setWindowTitle(QApplication::translate("ChangeEmpireDialog", "Form", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ChangeEmpireDialog", "Select Empire", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ChangeEmpireDialog);
    } // retranslateUi

};

namespace Ui {
    class ChangeEmpireDialog: public Ui_ChangeEmpireDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANGEEMPIREDIALOG_H
