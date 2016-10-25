#ifndef TRUCKWIDGET_H
#define TRUCKWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QTableWidget>
#include <QPushButton>

class DES;

class TruckInfo : public QObject
{
    Q_OBJECT
public:
    TruckInfo() : m_id(0), m_brand(""), m_driver(""), m_isFree(true) {}

    int m_id;
    QString m_brand;
    QString m_driver;
    bool m_isFree;

};

class TruckWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TruckWidget(QWidget *parent = 0);

private:
    void initTable();

    QTableWidget *truckTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *applyBtn;
    QPushButton *cancelBtn;
    QPushButton *readBtn;
    QPushButton *saveBtn;

    TruckInfo *temp;
    QList<TruckInfo *> trucks;
    DES *des;

    bool editMode;

signals:
    void addTruck(TruckInfo *truck);
    void delTruck(int index);

private slots:
    void addRecord();
    void delRecord();
    void applyRecord();
    void cancelRecord();
    void readRecord();
    void saveRecord();

public slots:
    void truckBack(QString &truckId);
    void truckGo(QString &truckId);
};

#endif // TRUCKWIDGET_H
