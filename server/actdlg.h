#ifndef ACTDLG_H
#define ACTDLG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class Activate;
QT_END_NAMESPACE

namespace Ui {
class ActDlg;
}

class ActDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ActDlg(Activate *a, QWidget *parent = 0);
    ~ActDlg();

private slots:
    void on_okBtn_clicked();

private:
    Ui::ActDlg *ui;
    Activate *act;
};

#endif // ACTDLG_H
