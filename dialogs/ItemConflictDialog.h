#ifndef ITEMCONFLICTDIALOG_H
#define ITEMCONFLICTDIALOG_H

#include "SNItem.h"

#include <QDialog>

namespace Ui {
    class ItemConflictDialog;
}
namespace SN
{
    enum ItemConflict {
        KeepOldItem = 2,
        KeepNewItem,
        KeepOldItemAndRepeat,
        KeepNewItemAndRepeat
    };
}
class ItemConflictDialog : public QDialog {
    Q_OBJECT
public:
    ItemConflictDialog( const SNItem &oldold, const SNItem &newitem, QWidget *parent = 0 );
    ~ItemConflictDialog();


protected:
    void changeEvent(QEvent *e);

private:

    void populateFields();
    void showDiffs();

    Ui::ItemConflictDialog *m_ui;

    SNItem m_oldItem;
    SNItem m_newItem;

private slots:
    void on_saveNewBut_clicked();
    void on_saveOldBut_clicked();
};

#endif // ITEMCONFLICTDIALOG_H
