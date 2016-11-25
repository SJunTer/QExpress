#ifndef TRUCKWIDGET_H
#define TRUCKWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QPushButton;
class Dbsql;
QT_END_NAMESPACE

#define FREE 0
#define MOVE 1

// 车辆信息结构体
struct TruckInfo
{
    int id;
    QString brand;    //型号
    QString LPN;   //车牌号
    int status;
};

class TruckWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TruckWidget(Dbsql *d, QWidget *parent = 0);
    ~TruckWidget();

    void readInfo();

private:
    void initTable();

    Dbsql *dbsql;
    QTableWidget *truckTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *applyBtn;

    TruckInfo *temp;
    QList<TruckInfo *> trucks;

    bool editMode;

signals:
    void sendTrucks(QList<TruckInfo*>*ts);

private slots:
    void addRecord();
    void delRecord();
    void applyRecord();

public slots:
    void truckMove(int id);
    void truckFree(int id);
};

#endif // TRUCKWIDGET_H
