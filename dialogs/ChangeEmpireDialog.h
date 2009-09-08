#ifndef CHANGEEMPIREDIALOG_H
#define CHANGEEMPIREDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class ChangeEmpireDialog;
}

class EmpiresModel;
class Empire;
class ChangeEmpireDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ChangeEmpireDialog)
public:
    explicit ChangeEmpireDialog(QWidget *parent = 0);
    virtual ~ChangeEmpireDialog();

signals:
    void currEmpireChanged( const Empire& );

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ChangeEmpireDialog *m_ui;
    EmpiresModel *m_empiresModel;

private slots:
    void on_buttonBox_accepted();
};

#endif // CHANGEEMPIREDIALOG_H
