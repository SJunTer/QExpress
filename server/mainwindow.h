#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>

QT_BEGIN_NAMESPACE
class TabWidget;
class MapWidget;
class DeliveryWidget;
class TruckWidget;
class InventoryWidget;
class AccWidget;
class TcpServer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    TabWidget *tabWidget;
    MapWidget *mapWidget;
    DeliveryWidget *deliveryWidget;
    AccWidget *accWidget;
    TruckWidget *truckWidget;
    InventoryWidget *inventoryWidget;
    TcpServer *server;
    void runServer();

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *e);

signals:
    void stopServer();

private slots:
    void enterSelectMode();
    void enterCargoMode();
    void transferData(QVector<long> &points, QVector<long> &path, QStringList &nameList);
    void sendTitles(QStringList &titles);
};

#endif // MAINWINDOW_H
