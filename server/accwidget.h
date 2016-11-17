#ifndef ACCWIDGET_H
#define ACCWIDGET_H

#include <QWidget>
#include <QString>
#include <QTime>
#include <QList>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
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
    bool status;
};

class AccWidget : public QWidget
{
    Q_OBJECT
public:
    AccWidget(QWidget *parent = 0);
    ~AccWidget();

private:
    void initTable();

    QTableWidget *accTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *sendBtn;
    QPushButton *closeBtn;

    QList<Account *> accounts;


signals:

private slots:
    void addAccount();
    void delAccount();
    void sendMsg();
    void closeConn();

public slots:
    void signIn(int id);
    void signOut(int id);
};



#endif // ACCWIDGET_H
