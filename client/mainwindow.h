#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QToolButton;
class QPushButton;
class MapView;
class ClientSocket;
class TaskDlg;
class AboutDlg;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent, ClientSocket *cli);

private:
    MapView *view;
    ClientSocket *client;

    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;
    QToolButton *taskBtn;
    QToolButton *uploadBtn;
    QToolButton *userBtn;
    QToolButton *aboutBtn;

    TaskDlg *taskDlg;
    AboutDlg *aboutDlg;

    bool uploadDlgShowed;
    bool userDlgShowed;
    bool taskRun;

protected:
    void closeEvent(QCloseEvent *e);

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

public slots:
    void setTaskState(bool b) { taskRun = b; }

};

#endif // MAINWINDOW_H
