#ifndef ACCWIDGET_H
#define ACCWIDGET_H

#include <QWidget>
#include <QString>
#include <QTime>
#include <QList>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
class Dbsql;
QT_END_NAMESPACE

#define ONLINE 1
#define OFFLINE 0

// 用户信息
struct Account
{
    int id;
    QString username;
    QString password;
    QString name;
    QString phone;
    QString email;
    QTime last_time;
    int status;
};

class AccWidget : public QWidget
{
    Q_OBJECT
public:
    AccWidget(Dbsql *d, QWidget *parent = 0);
    ~AccWidget();

    void readInfo();

private:
    void initTable();
    QTableWidget *accTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *closeBtn;

    QList<Account> accounts;
    Dbsql *dbsql;

private slots:
    void addAccount();
    void delAccount();
    void sendMsg();
    void closeConn();

public slots:
    void signIn(int id);
    void signUp(Account a);
    void signOut(int id);
    void changeInfo(Account a);
};



#endif // ACCWIDGET_H
