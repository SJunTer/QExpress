#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define BTN_SIZE 25


QT_BEGIN_NAMESPACE
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
    QPushButton *orderBtn;
    QPushButton *trafficBtn;
    QPushButton *userBtn;
    QPushButton *aboutBtn;

private slots:


};

#endif // MAINWINDOW_H
