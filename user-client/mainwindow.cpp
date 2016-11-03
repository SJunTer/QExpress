#include "mainwindow.h"
#include "mapwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mapWidget = new MapView(this);

    setCentralWidget(mapWidget);
}
