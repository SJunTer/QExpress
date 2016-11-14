#include "mainwindow.h"
#include "mapview.h"
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent, ClientSocket *cli)
    : QMainWindow(parent)
    , client(cli)
{
    mapView = new MapView(this, cli);

    setWindowFlags(Qt::Widget);
    setWindowTitle(tr("QExpress"));
    setCentralWidget(mapView);
    setMinimumSize(800, 600);

}
