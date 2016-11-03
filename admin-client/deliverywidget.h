#ifndef DELIVERYWIDGET_H
#define DELIVERYWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QList>
#include <QStringList>

class DeliveryPath;
class TruckInfo;

class DeliveryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeliveryWidget(QWidget *parent = 0);
    void setCargo(QStringList &titles);

private:
    QTableWidget *deliveryTable;
    QListWidget *pathList;

    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *selectBtn;
    QPushButton *cargoBtn;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    QList<TruckInfo *> trucks;

    QList<DeliveryPath *> paths;
    DeliveryPath *tempPath;
    bool editMode;

    void initTable();   //初始化表格
    void showPath(DeliveryPath *p);    // 显示暂存路径

    void updateCombo();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void selectMode();   //发送切换视图信号
    void cargoMode();
    void drawPath(DeliveryPath *path);
    void removePath(int index);
    void updatePath(int index, DeliveryPath *path);
    void truckBack(QString &truckId);
    void truckGo(QString &truckId);

private slots:
    void addLine(); // 添加行
    void delLine(); // 删除行
    void selectPoints(); // 选点模式
    void selectCargo(); // 选择货物
    void applyPath(); // 应用当前配送方案
    void cancel(); // 放弃当前方案

    void showPath(int curRow, int curCol, int preRow, int preCol); // 显示路径

public slots:
    void setPath(QVector<long> &points, QVector<long> &path, QStringList &nameList); // 创建路径对象
    void posChanged(int pos, int index);
    void addTruck(TruckInfo *truck);
    void delTruck(int index);
};

#endif // DELIVERYWIDGET_H
