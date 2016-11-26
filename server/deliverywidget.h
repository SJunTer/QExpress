#ifndef DELIVERYWIDGET_H
#define DELIVERYWIDGET_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QVector>
#include <QTime>
#include <QStringList>


// 配送状态
#define WAIT 0
#define RUN 1
#define FINISH 2
#define INTR 3


//地点类型
enum PlaceType {
    IsRepo, IsDely, IsPass
};


// 途经点
struct Place
{
    QString title;  // 地名
    PlaceType type; //类型
    QPointF coord;    // 坐标
};

// 路径信息
struct DeliveryPath
{
    int id;
    int truckId;
    int driverId;
    QStringList cargos;
    QList<Place> places;
    int pos;    // 当前位置
    int status;     // 当前状态
    QString moveTime;   // 运行时间
    QString startTime;    // 出发时间
};


QT_BEGIN_NAMESPACE
class QTableWidget;
class QListWidget;
class QPushButton;
class Dbsql;
class DelvDlg;
class Account;
class CargoInfo;
class TruckInfo;
QT_END_NAMESPACE

class DeliveryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeliveryWidget(Dbsql *d, QWidget *parent = 0);
    void setCargo(QStringList &titles);

    void readInfo();

private:
    void initTable();   //初始化表格

    Dbsql *dbsql;

    QTableWidget *deliveryTable;
    QListWidget *pathList;

    QPushButton *addBtn;
    QPushButton *delBtn;

    QList<Account> *accounts;
    QList<CargoInfo *> *cargos;
    QList<TruckInfo *> *trucks;

    QList<DeliveryPath *> paths;

    DelvDlg *delvDlg;
    bool delvDlgShowed;

signals:
    void selectMode();   //发送切换视图信号
    void drawPath(DeliveryPath *path);
    void updatePath(DeliveryPath *path);
    void removePath(int id);

    void sendTask(DeliveryPath path);

    void driverWork(int id);
    void driverFree(int id);
    void truckMove(int id);
    void truckFree(int id);
    void cargoMove(QStringList cs);
    void cargoReach(QStringList cs);
    void cargoFail(QStringList cs);

    void taskMsg(QString &s);

private slots:
    void on_addBtn_clicked();
    void on_delBtn_clicked();

    void showPath(int curRow, int curCol, int preRow, int preCol); // 显示路径

public slots:
    void setPath(QList<Place> &places); // 创建路径对象

    void acptTask(int id);
    void posChanged(int id, int pos);
    void taskFinish(int id);
    void taskFail(int id);

    void closeDelvDLg();
    void addPath(DeliveryPath *path);

    void setAccounts(QList<Account>*as) { accounts = as; }
    void setCargos(QList<CargoInfo*>*cs) { cargos = cs; }
    void setTrucks(QList<TruckInfo*>*ts) { trucks = ts; }

};

#endif // DELIVERYWIDGET_H
