#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QMainWindow>
#include <QVector>
#include <QList>
#include <QStringList>
#include "deliverywidget.h"

QT_BEGIN_NAMESPACE
class DeliveryPath;
class QListWidget;
class QCheckBox;
class QPushButton;
class MapView;
QT_END_NAMESPACE


class MapWidget : public QMainWindow
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit MapWidget(QWidget *parent = 0);

    void enterSelectMode();

private:
    MapView *view;
    QList<DeliveryPath *> paths;

    QPushButton *tileBtn;   //切片窗口
    QPushButton *zoomInBtn; //放大按钮
    QPushButton *zoomOutBtn; //缩小按钮

signals:
    void transferData(QList<Place> &places);

private slots:
    void makeTile();

public slots:
    void addPath(DeliveryPath *path);
    void removePath(int index);
    void updatePath(int index, DeliveryPath *path);
};

#endif // MAPWIDGET_H
