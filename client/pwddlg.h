#ifndef PWDDLG_H
#define PWDDLG_H

#include <QDialog>
#include <QString>

QT_BEGIN_NAMESPACE
class ClientSocket;
QT_END_NAMESPACE


namespace Ui {
class PwdDlg;
}

class PwdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PwdDlg(QString &usr, ClientSocket *cli, QWidget *parent = 0);
    ~PwdDlg();

private:
    Ui::PwdDlg *ui;
    QString usrnm;
    ClientSocket *client;

private slots:
    void on_okBtn_clicked();
};

#endif // PWDDLG_H
