#ifndef NEWEMPIREDIALOG_H
#define NEWEMPIREDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class NewEmpireDialog;
}

class NewEmpireDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(NewEmpireDialog)
public:
    explicit NewEmpireDialog(QWidget *parent = 0);
    virtual ~NewEmpireDialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::NewEmpireDialog *m_ui;

    bool setupNewDatabase( const QString &name, const QString &id );

private slots:
    void on_acceptButton_clicked();
    void on_empireIDEdit_textEdited(QString );
};

#endif // NEWEMPIREDIALOG_H
