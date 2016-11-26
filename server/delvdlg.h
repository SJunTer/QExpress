#ifndef DELVDLG_H
#define DELVDLG_H

#include <QDialog>
#include <QList>
#include <QVector>
#include "deliverywidget.h"

#define CARGO_WIDGET 0
#define TRUCK_WIDGET 1
#define ACC_WIDGET 2
#define POINT_WIDGET 3

QT_BEGIN_NAMESPACE
class QPushButton;
class QTableWidget;
class QListWidget;
class QLabel;
class QCheckBox;
QT_END_NAMESPACE


class CheckPair : public QObject
{
    Q_OBJECT
public:
    QCheckBox *box;
    bool checked;
    int index;
    int id;
    QString s;
signals:
    void boxChecked(bool, int index);
public slots:
    void stateChanged(int state);
};

class CargoSelectWidget : public QWidget
{
    Q_OBJECT
public:
    CargoSelectWidget(DeliveryPath *p, QList<CargoInfo*>*cs, QWidget *parent = 0);
    ~CargoSelectWidget();

private:
    DeliveryPath *path;
    QList<CargoInfo *> *cargos;

    QLabel *tipLabel;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    QPushButton *cancelBtn;
    QTableWidget *cargoTable;

    QVector<CheckPair*> boxes;

    void initTable();


signals:
    void jump(int index);
    void cancel();

private slots:
    void on_nextBtn_clicked();
    void on_cancelBtn_clicked();
};
\
class TruckSelectWidget : public QWidget
{
    Q_OBJECT
public:
    TruckSelectWidget(DeliveryPath *p, QList<TruckInfo*>*ts, QWidget *parent = 0);
    ~TruckSelectWidget();

private:
    DeliveryPath *path;
    QList<TruckInfo *> *trucks;

    QLabel *tipLabel;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    QPushButton *cancelBtn;
    QTableWidget *truckTable;

    QVector<CheckPair*> boxes;
    int currIndex;

    void initTable();

signals:
    void jump(int index);
    void cancel();

private slots:
    void on_prevBtn_clicked();
    void on_nextBtn_clicked();
    void on_cancelBtn_clicked();
    void boxSelected(bool b, int index);

};

class AccSelectWidget : public QWidget
{
    Q_OBJECT
public:
    AccSelectWidget(DeliveryPath *p, QList<Account>*as, QWidget *parent = 0);
    ~AccSelectWidget();

private:
    DeliveryPath *path;
    QList<Account> *accounts;

    QLabel *tipLabel;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    QPushButton *cancelBtn;
    QTableWidget *accTable;

    QVector<CheckPair *> boxes;
    int currIndex;

    void initTable();

signals:
    void jump(int index);
    void cancel();

private slots:
    void on_prevBtn_clicked();
    void on_nextBtn_clicked();
    void on_cancelBtn_clicked();
    void boxSelected(bool b, int index);
};

class PointSelectWidget : public QWidget
{
    Q_OBJECT
public:
    PointSelectWidget(DeliveryPath *p, QWidget *parent = 0);

private:
    DeliveryPath *path;

    QLabel *tipLabel;
    QPushButton *selectBtn;
    QPushButton *prevBtn;
    QPushButton *finishBtn;
    QPushButton *cancelBtn;
    QListWidget *pointList;

    void initList();

signals:
    void selectPoint();

    void jump(int index);
    void finish();
    void cancel();

private slots:
    void on_prevBtn_clicked();
    void on_finishBtn_clicked();
    void on_cancelBtn_clicked();

public slots:
    void setPath(QList<Place> &places);
};

/****************************
 *                  对话框容器                      *
 * **************************/
class DelvDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DelvDlg(QList<Account>*as, QList<CargoInfo*>*cs,
                     QList<TruckInfo*>*ts,QWidget *parent = 0);

    void setPath(QList<Place> &places);

private:
    DeliveryPath *path;

    CargoSelectWidget *cargoSelectWidget;
    TruckSelectWidget *truckSelectWidget;
    AccSelectWidget *accSelectWidget;
    PointSelectWidget *pointSelectWidget;


protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void closed();
    void selectPoint();
    void addPath(DeliveryPath *path);

public slots:
    void closeDlg();
    void finish();

    void hideAndSelect();

};


#endif // DELVDLG_H
