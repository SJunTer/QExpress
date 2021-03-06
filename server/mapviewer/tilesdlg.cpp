#include "tilesdlg.h"
#include "unistd.h"
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QProgressBar>
#include <QGraphicsScene>
#include <QBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QThread>
#include <QDebug>


//---------------------------------TileMaker-----------------------------------
//开始执行切片任务
void TileMaker::doWork()
{
    qDebug() << "do work";

    QPixmap pix(256, 256);
    QPainter painter(&pix);

    int max_side = pow(2, 18);
    int paddingH = (max_side - scene->itemsBoundingRect().width())/2;
    int paddingV = (max_side - scene->itemsBoundingRect().height())/2;
    int cornerX = scene->itemsBoundingRect().x() - paddingH;
    int cornerY = scene->itemsBoundingRect().y() - paddingV;
    //    qDebug() << scene->itemsBoundingRect();
    //    qDebug() << cornerX << cornerY;
    //    qDebug() << (scene->itemsBoundingRect().width()+2*paddingH)/pow(2, 10);
    //    qDebug() << (scene->itemsBoundingRect().height()+2*paddingV)/pow(2, 10);
    const int grid[] = { 10,10,10,10,10,10,10,12,16,23,32,45 };

    makeDir("tiles");
    for(currLevel = minLevel; !stopped && currLevel <= maxLevel; ++currLevel)
    {

        makeDir(QString("tiles/%1").arg(currLevel));
        emit setLayerVisible(currLevel);
        sleep(1);
        //make dirs
        for(int row = 0; !stopped && row*grid[currLevel-MIN_LEVEL] < pow(2, currLevel-MIN_LEVEL); ++row)
        {
            for(int col = 0; col*grid[currLevel-MIN_LEVEL] < pow(2, currLevel-MIN_LEVEL); ++col)
            {
                if(stopped)
                    break;
                makeDir(QString("tiles/%1/%2_%3").arg(currLevel).arg(row).arg(col));
            }
        }

        //make tiles
        int step = pow(2, 8 + MAX_LEVEL - currLevel);
        for(int row = 0; row < pow(2, currLevel-MIN_LEVEL); row+=1)
        {
            for(int col = 0; col < pow(2, currLevel-MIN_LEVEL); col+=1)
            {
                pix.fill(Qt::white);
                scene->render(&painter, QRectF(), QRect(cornerX+col*step, cornerY+row*step, step, step));
                QString filename = QString("tiles/%1/%2_%3/%4_%5.png").arg(currLevel).
                        arg(row/grid[currLevel-MIN_LEVEL]).arg(col/grid[currLevel-MIN_LEVEL]).arg(row).arg(col);
                pix.save(filename);
                ++cnt;
                emit updateProgress(cnt);
            }
        }
    }

    emit setLayerVisible(0);  //restore zoom
    emit updateProgress(cnt);
    emit taskFinished();
    //    exit();
}

void TileMaker::stop()
{
    stopped = true;
}

void TileMaker::makeDir(const QString &folderName)
{
    QDir dir(folderName);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }
}

//-------------------------------------TilesDlg-----------------------------------

TilesDlg::TilesDlg(QWidget *parent, QGraphicsScene *s)
    : QDialog(parent)
    , scene(s)
    , running(false)
    , nComplete(0)
    , nTotal(0)
    , secs(0)
{
    label = new QLabel(this);
    proBar = new QProgressBar(this);
    startBtn = new QPushButton(this);
    cancelBtn = new QPushButton(this);

    QFont font;
    font.setPixelSize(14);
    label->setText("未开始");
    label->setFont(font);
    startBtn->setText("开始");
    startBtn->setFixedSize(80, 25);
    cancelBtn->setText("取消");
    cancelBtn->setFixedSize(80,25);
    proBar->setFixedHeight(30);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(5);
    btnLayout->setMargin(0);
    btnLayout->addStretch();
    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10,10,10, 10);
    mainLayout->addWidget(label, 0, Qt::AlignLeft);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(proBar);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    setFixedSize(300,130);
    setWindowTitle(tr("切片生成工具"));

    connect(startBtn, SIGNAL(clicked(bool)), this, SLOT(start()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(cancel()));

}

// 获得切片总数
int TilesDlg::getTotalCnt(int minLevel, int maxLevel)
{
    int sum = 0;
    for(int lv = minLevel; lv <= maxLevel; ++lv)
        sum += pow(4, lv-MIN_LEVEL);
    qDebug() << sum;
    return sum;
}

// 开始生成接片
void TilesDlg::start()
{
    startBtn->setEnabled(false);
    running = true;
    nComplete = 0;
    int minLevel = 15;
    int maxLevel = 15;
    nTotal = getTotalCnt(minLevel, maxLevel);
    proBar->setRange(0, nTotal-1);
    label->setText(QString("0/%1--0%").arg(nTotal));

    // 开启制作切片线程
    QThread *thread = new QThread();
    TileMaker *tileMaker = new TileMaker(minLevel, maxLevel,scene);
    tileMaker->moveToThread(thread);
    connect(thread, SIGNAL(started()), tileMaker, SLOT(doWork()));
    connect(tileMaker, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), tileMaker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    connect(tileMaker, SIGNAL(taskFinished()), this, SLOT(taskFinished()));
    connect(tileMaker, SIGNAL(updateProgress(int)), this, SLOT(updateProgress(int)));
    connect(tileMaker, SIGNAL(setLayerVisible(int)), this, SIGNAL(setLayerVisible(int)));
    connect(this, SIGNAL(stopTask()), tileMaker, SLOT(stop()), Qt::DirectConnection);
    thread->start();

}

// 取消
void TilesDlg::cancel()
{
    if(!running)
    {
        this->close();
        return;
    }
    if(QMessageBox::warning(this, "警告", "切片任务正在进行，是否确定退出？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    emit stopTask();
    this->close();
}

void TilesDlg::updateProgress(int n)
{
    nComplete = n;
    proBar->setValue(n);
    label->setText(QString("%1/%2----%3%").arg(nComplete).arg(nTotal).arg((double)nComplete/nTotal*100.0));
}

void TilesDlg::taskFinished()
{
    running = false;
}
