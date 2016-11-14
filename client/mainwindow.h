#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "../network/socket.h"

class MapView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent, ClientSocket *cli);

private:
    MapView *mapView;
    ClientSocket *client;
};

#endif // MAINWINDOW_H
