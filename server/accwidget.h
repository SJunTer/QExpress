#ifndef ACCWIDGET_H
#define ACCWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
class Dbsql;
QT_END_NAMESPACE

#define WORK 2
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
    QString last_time;
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

signals:
    void closeClient(int id);
    void sendAccounts(QList<Account> *as);

private slots:
    void addAccount();
    void delAccount();
    void closeConn();

public slots:
    void signIn(int id);
    void signUp(Account a);
    void signOut(int id);
    void changeInfo(Account a);

    void driverWork(int id);
    void driverFree(int id);
};



#endif // ACCWIDGET_H
