#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QToolButton;
class QPushButton;
class MapView;
class ClientSocket;
class PathWidget;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent, ClientSocket *cli);

private:
    MapView *view;
    PathWidget *pathWidget;
    ClientSocket *client;

    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;
    QToolButton *orderBtn;
    QToolButton *trafficBtn;
    QToolButton *userBtn;
    QToolButton *aboutBtn;

private slots:
    void getOrder();
    void UploadTraffic();
    void userSetting();
    void about();

};

#endif // MAINWINDOW_H
