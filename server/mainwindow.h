#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include "deliverywidget.h"

QT_BEGIN_NAMESPACE
class TabWidget;
class MapWidget;
class DeliveryWidget;
class TruckWidget;
class CargoWidget;
class AccWidget;
class TcpServer;
class AboutDlg;
class MessageDlg;
class QToolButton;
class Dbsql;
class ServerSocket;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(ServerSocket *s, Dbsql *d, QWidget *parent = 0);
    ~MainWindow();

private:
    TabWidget *tabWidget;
    MapWidget *mapWidget;
    DeliveryWidget *deliveryWidget;
    AccWidget *accWidget;
    TruckWidget *truckWidget;
    CargoWidget *cargoWidget;

    MessageDlg *msgDlg;
    QToolButton *msgBtn;
    AboutDlg *aboutDlg;
    QToolButton *aboutBtn;

    ServerSocket *servSock;
    TcpServer *tcpServer;
    Dbsql *dbsql;
    void runServer();


protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *e);

signals:
    void stopServer();

private slots:
    void showMsgDlg();
    void showAboutDlg();

    void enterSelectMode();
    void selectDone(QList<Place> &places);
};

#endif // MAINWINDOW_H
