#ifndef MESSAGEDLG_H
#define MESSAGEDLG_H

#include <QDialog>
#include <QList>
#include <QString>
#include "accwidget.h"


#define ACCIDENT 0
#define JAM 1
#define BLOCK 2


namespace Ui {
class MessageDlg;
}

class MessageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDlg(QWidget *parent = 0);
    ~MessageDlg();

private:
    Ui::MessageDlg *ui;
    QList<Account> accounts;

    void initTable();

signals:
    void signUp(Account a);

private slots:
    void on_acceptBtn_clicked();
    void on_rejectBtn_clicked();

public slots:
    void signUpReq(Account a);
    void getUpload(QString usr, int type, QString addr, QString detail);
};

#endif // MESSAGEDLG_H
