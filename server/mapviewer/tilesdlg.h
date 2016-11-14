#ifndef TILESDLG_H
#define TILESDLG_H

#include <QDialog>
#include <QString>

#define MAX_LEVEL 18
#define MIN_LEVEL 8

QT_BEGIN_NAMESPACE
class QProgressBar;
class QGraphicsScene;
class QPushButton;
class QLabel;
QT_END_NAMESPACE


class TileMaker : public QObject
{
    Q_OBJECT
public:
    TileMaker(int lv, QGraphicsScene *s)
        : cnt(0), maxLevel(lv), stopped(false), scene(s) {}

private:
    int cnt;
    int currLevel;
    int maxLevel;
    bool stopped;
    QGraphicsScene *scene;

    void exit();
    void makeDir(const QString &folderName);

signals:
    void setLayerVisible(int level);
    void updateProgress(int n);
    void taskFinished();

public slots:
    void doWork();
    void stop();

};

class TilesDlg : public QDialog
{
    Q_OBJECT
public:
    TilesDlg(QWidget *parent, QGraphicsScene *s);

private:
    QLabel *label;
    QProgressBar *proBar;
    QPushButton *startBtn;
    QPushButton *cancelBtn;
    QGraphicsScene *scene;

    bool running;   // 开始标志
    int nComplete;  // 已完成切片数
    int nTotal;     // 总共所需切片数
    int secs;   // 已经过的秒数

    int getTotalCnt(int maxLevel);

signals:
    void setLayerVisible(int level);
    void stopTask();

private slots:
    void start();
    void cancel();

public slots:
    void updateProgress(int n);
    void taskFinished();

};

#endif // TILESDLG_H
