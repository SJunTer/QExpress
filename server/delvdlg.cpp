#include "delvdlg.h"
#include "accwidget.h"
#include "cargowidget.h"
#include "truckwidget.h"
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QStringList>
#include <QListWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QBoxLayout>
#include <QStackedLayout>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>


void CheckPair::stateChanged(int state)
{
    if(state == Qt::Unchecked)
    {
        checked = false;
        emit boxChecked(false, index);
    }
    else
    {
        checked = true;
        emit boxChecked(true, index);
    }
}


DelvDlg::DelvDlg(QList<Account>*as, QList<CargoInfo*>*cs,
                 QList<TruckInfo*>*ts,QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("配送任务选择面板"));
    setFixedSize(650,450);

    path = new DeliveryPath;

    cargoSelectWidget = new CargoSelectWidget(path, cs, this);
    truckSelectWidget = new TruckSelectWidget(path, ts, this);
    accSelectWidget = new AccSelectWidget(path, as, this);
    pointSelectWidget = new PointSelectWidget(path, this);

    QStackedLayout *stackLayout = new QStackedLayout;
    stackLayout->insertWidget(0, cargoSelectWidget);
    stackLayout->insertWidget(1, truckSelectWidget);
    stackLayout->insertWidget(2, accSelectWidget);
    stackLayout->insertWidget(3, pointSelectWidget);

    stackLayout->setCurrentIndex(0);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(10);
    layout->addLayout(stackLayout);
    setLayout(layout);

    qDebug() << as->size() << cs->size() << ts->size();

    connect(cargoSelectWidget, SIGNAL(cancel()), this, SLOT(closeDlg()));
    connect(truckSelectWidget, SIGNAL(cancel()), this, SLOT(closeDlg()));
    connect(accSelectWidget, SIGNAL(cancel()), this, SLOT(closeDlg()));
    connect(pointSelectWidget, SIGNAL(cancel()), this, SLOT(closeDlg()));
    connect(cargoSelectWidget, SIGNAL(jump(int)), stackLayout, SLOT(setCurrentIndex(int)));
    connect(truckSelectWidget, SIGNAL(jump(int)), stackLayout, SLOT(setCurrentIndex(int)));
    connect(accSelectWidget, SIGNAL(jump(int)), stackLayout, SLOT(setCurrentIndex(int)));
    connect(pointSelectWidget, SIGNAL(jump(int)), stackLayout, SLOT(setCurrentIndex(int)));
    connect(pointSelectWidget, SIGNAL(selectPoint()), this, SLOT(hideAndSelect()));
    connect(pointSelectWidget, SIGNAL(finish()), this, SLOT(finish()));

}

void DelvDlg::setPath(QList<Place> &places)
{
    pointSelectWidget->setPath(places);
}

void DelvDlg::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else
    {
        e->ignore();
    }

}

void DelvDlg::closeDlg()
{
    delete path;
    emit closed();
    this->close();
}

void DelvDlg::finish()
{
    emit addPath(path);
    emit closed();
    this->close();
}

void DelvDlg::hideAndSelect()
{
    this->hide();
    emit selectPoint();
}


/***********货物选择***********/
CargoSelectWidget::CargoSelectWidget(DeliveryPath *p, QList<CargoInfo *> *cs, QWidget *parent) :
    QWidget(parent),
    path(p),
    cargos(cs)
{
    tipLabel = new QLabel(this);
    prevBtn = new QPushButton(this);
    nextBtn = new QPushButton(this);
    cancelBtn  = new QPushButton(this);
    cargoTable = new QTableWidget(this);

    QFont font;
    font.setPixelSize(15);
//    font.setBold(true);
    tipLabel->setText("请选择需要进行配送的货物");
    tipLabel->setFont(font);
    tipLabel->setFixedHeight(35);
    prevBtn->setText("上一步");
    prevBtn->setFixedSize(80,25);
    prevBtn->setEnabled(false);
    nextBtn->setText("下一步");
    nextBtn->setFixedSize(80,25);
    cancelBtn->setText("取消");
    cancelBtn->setFixedSize(80,25);
    initTable();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);
    btnLayout->addStretch();
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(nextBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(tipLabel);
    layout->addWidget(cargoTable);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(nextBtn, SIGNAL(clicked(bool)), this, SLOT(on_nextBtn_clicked()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(on_cancelBtn_clicked()));
}

CargoSelectWidget::~CargoSelectWidget()
{
    for(int i = 0; i < boxes.size(); ++i)
    {
        CheckPair *c = boxes[i];
        delete c;
    }
}

void CargoSelectWidget::initTable()
{
    cargoTable->setColumnCount(5);
//    cargoTable->horizontalHeader()->setDefaultSectionSize(150);
    cargoTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header  << tr("选择") << tr("序号") << tr("名称") << tr("目的地") << tr("收件人");
    cargoTable->setHorizontalHeaderLabels(header);

    cargoTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    cargoTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
    cargoTable->verticalHeader()->setVisible(false); //设置垂直头不可见
    cargoTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    cargoTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    cargoTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    cargoTable->horizontalHeader()->resizeSection(0,40); //设置表头第一列的宽度为50
    cargoTable->horizontalHeader()->setFixedHeight(25); //设置表头的高度

    for(int i = 0; i < cargos->size(); ++i)
    {
        if((*cargos)[i]->status != InRepo)
            continue;

        int rowCnt = cargoTable->rowCount();
        cargoTable->insertRow(rowCnt);

        QCheckBox *item1 = new QCheckBox;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;

        item1->setText("");
        item2->setText(QString::number((*cargos)[i]->id));
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText((*cargos)[i]->descrip);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText((*cargos)[i]->dest);
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
        item5->setText((*cargos)[i]->reciver);
        item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));

        cargoTable->setCellWidget(rowCnt, 0, item1);
        cargoTable->setItem(rowCnt, 1, item2);
        cargoTable->setItem(rowCnt, 2, item3);
        cargoTable->setItem(rowCnt, 3, item4);
        cargoTable->setItem(rowCnt, 4, item5);
        cargoTable->setCurrentCell(rowCnt, 0);

        CheckPair *pair = new CheckPair;
        pair->index = boxes.size();
        pair->s = (*cargos)[i]->descrip;
        pair->box = item1;
        pair->checked =false;
        connect(item1, SIGNAL(stateChanged(int)), pair, SLOT(stateChanged(int)));
        boxes.append(pair);
    }
}


void CargoSelectWidget::on_nextBtn_clicked()
{
    bool none = true;
    path->cargos.clear();
    for(int i = 0; i < boxes.size(); ++i)
    {
        if(boxes[i]->checked)
        {
            none = false;
            path->cargos.append(boxes[i]->s);
        }
    }

    if(none)
    {
        QMessageBox::warning(this, "错误", "请选择至少一项货物！", QMessageBox::Ok);
        return;
    }
    else
        emit jump(CARGO_WIDGET+1);
}

void CargoSelectWidget::on_cancelBtn_clicked()
{
    if(QMessageBox::warning(this, "警告", "是否取消选择配送任务？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    emit cancel();
}


/***********货车选择***********/
TruckSelectWidget::TruckSelectWidget(DeliveryPath *p, QList<TruckInfo *> *ts, QWidget *parent) :
    QWidget(parent),
    path(p),
    trucks(ts),
    currIndex(-1)
{
    tipLabel = new QLabel(this);
    prevBtn = new QPushButton(this);
    nextBtn = new QPushButton(this);
    cancelBtn  = new QPushButton(this);
    truckTable = new QTableWidget(this);

    QFont font;
    font.setPixelSize(15);
    tipLabel->setText("请选择配送车辆");
    tipLabel->setFont(font);
    tipLabel->setFixedHeight(35);
    prevBtn->setText("上一步");
    prevBtn->setFixedSize(80,25);
    nextBtn->setText("下一步");
    nextBtn->setFixedSize(80,25);
    cancelBtn->setText("取消");
    cancelBtn->setFixedSize(80,25);
    initTable();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);
    btnLayout->addStretch();
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(nextBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(tipLabel);
    layout->addWidget(truckTable);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(prevBtn, SIGNAL(clicked(bool)), this, SLOT(on_prevBtn_clicked()));
    connect(nextBtn, SIGNAL(clicked(bool)), this, SLOT(on_nextBtn_clicked()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(on_cancelBtn_clicked()));
}

TruckSelectWidget::~TruckSelectWidget()
{

    for(int i = 0; i < boxes.size(); ++i)
    {
        CheckPair *c = boxes[i];
        delete c;
    }
}

void TruckSelectWidget::initTable()
{
    truckTable->setColumnCount(4);
//    truckTable->horizontalHeader()->setDefaultSectionSize(150);
    truckTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header  << tr("选择") << tr("序号") << tr("型号") << tr("车牌号");
    truckTable->setHorizontalHeaderLabels(header);

    truckTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    truckTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
    truckTable->verticalHeader()->setVisible(false); //设置垂直头不可见
    truckTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    truckTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    truckTable->horizontalHeader()->resizeSection(0,40); //设置表头第一列的宽度为50
    truckTable->horizontalHeader()->setFixedHeight(25); //设置表头的高度

    for(int i = 0; i < trucks->size(); ++i)
    {
        if((*trucks)[i]->status != FREE)
            continue;

        int rowCnt = truckTable->rowCount();
        truckTable->insertRow(rowCnt);

        QCheckBox *item1 = new QCheckBox;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        item1->setText("");
        item2->setText(QString::number((*trucks)[i]->id));
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText((*trucks)[i]->brand);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText((*trucks)[i]->LPN);
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

        truckTable->setCellWidget(rowCnt, 0, item1);
        truckTable->setItem(rowCnt, 1, item2);
        truckTable->setItem(rowCnt, 2, item3);
        truckTable->setItem(rowCnt, 3, item4);
        truckTable->setCurrentCell(rowCnt, 0);

        CheckPair *pair = new CheckPair;
        pair->index = boxes.size();
        pair->id = (*trucks)[i]->id;
        pair->box = item1;
        pair->checked =false;
        connect(item1, SIGNAL(stateChanged(int)), pair, SLOT(stateChanged(int)));
        connect(pair, SIGNAL(boxChecked(bool,int)), this, SLOT(boxSelected(bool,int)));
        boxes.append(pair);
    }
}

void TruckSelectWidget::on_prevBtn_clicked()
{
    emit jump(TRUCK_WIDGET-1);
}

void TruckSelectWidget::on_nextBtn_clicked()
{
    if(currIndex == -1)
    {
        QMessageBox::warning(this, "错误", "请选择配送车辆！", QMessageBox::Ok);
        return;
    }
    path->truckId = boxes[currIndex]->id;
    emit jump(TRUCK_WIDGET+1);
}

void TruckSelectWidget::on_cancelBtn_clicked()
{
    if(QMessageBox::warning(this, "警告", "是否取消选择配送任务？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    emit cancel();
}

void TruckSelectWidget::boxSelected(bool b, int index)
{
    if(b)
    {
        if(currIndex != -1)
            boxes[currIndex]->box->setChecked(false);
        currIndex = index;
    }
    else
        currIndex = -1;
}

/***********账户选择***********/
AccSelectWidget::AccSelectWidget(DeliveryPath *p, QList<Account> *as, QWidget *parent) :
    QWidget(parent),
    path(p),
    accounts(as),
    currIndex(-1)
{
    tipLabel = new QLabel(this);
    prevBtn = new QPushButton(this);
    nextBtn = new QPushButton(this);
    cancelBtn  = new QPushButton(this);
    accTable = new QTableWidget(this);
    initTable();

    QFont font;
    font.setPixelSize(15);
    tipLabel->setText("请选择配送员");
    tipLabel->setFont(font);
    tipLabel->setFixedHeight(35);
    prevBtn->setText("上一步");
    prevBtn->setFixedSize(80,25);
    nextBtn->setText("下一步");
    nextBtn->setFixedSize(80,25);
    cancelBtn->setText("取消");
    cancelBtn->setFixedSize(80,25);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);
    btnLayout->addStretch();
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(nextBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(tipLabel);
    layout->addWidget(accTable);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(prevBtn, SIGNAL(clicked(bool)), this, SLOT(on_prevBtn_clicked()));
    connect(nextBtn, SIGNAL(clicked(bool)), this, SLOT(on_nextBtn_clicked()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(on_cancelBtn_clicked()));
}

AccSelectWidget::~AccSelectWidget()
{

    for(int i = 0; i < boxes.size(); ++i)
    {
        CheckPair *c = boxes[i];
        delete c;
    }
}

void AccSelectWidget::initTable()
{
    accTable->setColumnCount(6);
//    accTable->horizontalHeader()->setDefaultSectionSize(150);
    accTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header  << tr("选择") << tr("序号") << tr("用户名") << tr("姓名") << tr("电话") << tr("邮箱");
    accTable->setHorizontalHeaderLabels(header);

    accTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    accTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
    accTable->verticalHeader()->setVisible(false); //设置垂直头不可见
    accTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    accTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    accTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    accTable->horizontalHeader()->resizeSection(0,40); //设置表头第一列的宽度为50
    accTable->horizontalHeader()->setFixedHeight(25); //设置表头的高度

    for(int i = 0; i < accounts->size(); ++i)
    {
        if((*accounts)[i].status != ONLINE)
            continue;

        int rowCnt = accTable->rowCount();
        accTable->insertRow(rowCnt);

        QCheckBox *item1 = new QCheckBox;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;
        QTableWidgetItem *item6 = new QTableWidgetItem;

        item1->setText("");
        item2->setText(QString::number((*accounts)[i].id));
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText((*accounts)[i].username);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText((*accounts)[i].name);
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
        item5->setText((*accounts)[i].phone);
        item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));
        item6->setText((*accounts)[i].email);
        item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));

        accTable->setCellWidget(rowCnt, 0, item1);
        accTable->setItem(rowCnt, 1, item2);
        accTable->setItem(rowCnt, 2, item3);
        accTable->setItem(rowCnt, 3, item4);
        accTable->setItem(rowCnt, 4, item5);
        accTable->setItem(rowCnt, 5, item6);
        accTable->setCurrentCell(rowCnt, 0);

        CheckPair *pair = new CheckPair;
        pair->index = boxes.size();
        pair->id = (*accounts)[i].id;
        pair->box = item1;
        pair->checked =false;
        connect(item1, SIGNAL(stateChanged(int)), pair, SLOT(stateChanged(int)));
        connect(pair, SIGNAL(boxChecked(bool,int)), this, SLOT(boxSelected(bool,int)));
        boxes.append(pair);
    }
}

void AccSelectWidget::on_prevBtn_clicked()
{
    emit jump(ACC_WIDGET-1);
}

void AccSelectWidget::on_nextBtn_clicked()
{
    if(currIndex == -1)
    {
        QMessageBox::warning(this, "错误", "请选择配送员！", QMessageBox::Ok);
        return;
    }
    path->driverId = boxes[currIndex]->id;
    emit jump(ACC_WIDGET+1);
}

void AccSelectWidget::on_cancelBtn_clicked()
{
    if(QMessageBox::warning(this, "警告", "是否取消选择配送任务？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    emit cancel();
}

void AccSelectWidget::boxSelected(bool b, int index)
{
    if(b)
    {
        if(currIndex != -1)
            boxes[currIndex]->box->setChecked(false);
        currIndex = index;
    }
    else
        currIndex = -1;
}

/***********地点选择***********/
PointSelectWidget::PointSelectWidget(DeliveryPath *p, QWidget *parent) :
    QWidget(parent),
    path(p)
{
    tipLabel = new QLabel(this);
    selectBtn = new QPushButton(this);
    prevBtn = new QPushButton(this);
    finishBtn = new QPushButton(this);
    cancelBtn  = new QPushButton(this);
    pointList = new QListWidget(this);

    QFont font;
    font.setPixelSize(15);
    tipLabel->setText("请选择配送地点");
    tipLabel->setFont(font);
    selectBtn->setText("地图选点");
    selectBtn->setFixedSize(100,25);
    prevBtn->setText("上一步");
    prevBtn->setFixedSize(80,25);
    finishBtn->setText("完成");
    finishBtn->setFixedSize(80,25);
    cancelBtn->setText("取消");
    cancelBtn->setFixedSize(80,25);
    initList();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(5);
    btnLayout->addStretch();
    btnLayout->addWidget(prevBtn);
    btnLayout->addWidget(finishBtn);
    btnLayout->addWidget(cancelBtn);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setMargin(0);
    topLayout->setSpacing(0);
    topLayout->addWidget(tipLabel);
    topLayout->addStretch();
    topLayout->addWidget(selectBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(topLayout);
    layout->addSpacing(4);
    layout->addWidget(pointList);
    layout->addSpacing(6);
    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(selectBtn, SIGNAL(clicked(bool)), this, SIGNAL(selectPoint()));
    connect(prevBtn, SIGNAL(clicked(bool)), this, SLOT(on_prevBtn_clicked()));
    connect(finishBtn, SIGNAL(clicked(bool)), this, SLOT(on_finishBtn_clicked()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(on_cancelBtn_clicked()));
}

void PointSelectWidget::initList()
{

}

void PointSelectWidget::on_prevBtn_clicked()
{
    emit jump(POINT_WIDGET-1);
}

void PointSelectWidget::on_finishBtn_clicked()
{
    if(path->places.size() == 0)
    {
        QMessageBox::warning(this, "错误", "未选择配送点", QMessageBox::Ok);
        return;
    }
    emit finish();
}

void PointSelectWidget::on_cancelBtn_clicked()
{
    if(QMessageBox::warning(this, "警告", "是否取消选择配送任务？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    emit cancel();
}

void PointSelectWidget::setPath(QList<Place> &places)
{
    path->places = places;
    for(int i = 0; i < places.size(); ++i)
    {
        if(places[i].type != IsPass)
            pointList->addItem(places[i].title);
    }
}
