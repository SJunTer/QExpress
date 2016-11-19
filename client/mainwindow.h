#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QToolButton;
class QPushButton;
class MapView;
class ClientSocket;
class PathWidget;
class TaskDlg;
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
    QToolButton *taskBtn;
    QToolButton *uploadBtn;
    QToolButton *userBtn;
    QToolButton *aboutBtn;

    TaskDlg *taskDlg;

     bool uploadDlgShowed;
     bool userDlgShowed;
     bool aboutDlgShowed;

private slots:
    void on_taskBtn_clicked();
    void on_uploadBtn_clicked();
    void on_userBtn_clicked();
    void on_aboutBtn_clicked();
    void uploadDlgClosed() {
        uploadDlgShowed = false;
    }
    void userDlgClosed() {
        userDlgShowed = false;
    }
    void aboutDlgClosed() {
        aboutDlgShowed = false;
    }

};

#endif // MAINWINDOW_H
