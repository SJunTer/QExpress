#ifndef REGDLG_H
#define REGDLG_H

#include <QDialog>
#include "account.h"

namespace Ui {
class RegDlg;
}

class RegDlg : public QDialog
{
    Q_OBJECT
public:
    explicit RegDlg(QWidget *parent = 0);
    ~RegDlg();

private:
    Ui::RegDlg *ui;
    Account acc;

signals:
    bool reg(Account &acc);

private slots:
    void on_regBtn_clicked();
};

#endif // REGDLG_H
