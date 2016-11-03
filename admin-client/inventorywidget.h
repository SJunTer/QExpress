#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QStringList>

enum cargoStatus {
    InRepo, IsMove, IsComplete
};


class DES;

class CargoInfo : public QObject
{
    Q_OBJECT
public:
    CargoInfo() : m_isSelect(false), m_index(0), m_title(""),
        m_dest(""), m_reciver(""), m_status(InRepo) {}

    bool m_isSelect;

    int m_index;
    QString m_title;
    QString m_dest;
    QString m_reciver;
    cargoStatus m_status;

public slots:
    void stateChanged(int flag){
        m_isSelect = flag;
    }
};


class InventoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InventoryWidget(QWidget *parent = 0);

    void setSelectMode(bool mode) {
        selectMode = mode;
    }

private:
    void initTable();

    QTableWidget *cargoTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *applyBtn;
    QPushButton *cancelBtn;
    QPushButton *selectBtn;
    QPushButton *readBtn;
    QPushButton *saveBtn;

    CargoInfo *temp;
    QList<CargoInfo *> cargos;

    QStringList titles;

    bool selectMode;
    bool editMode;

    DES *des;

signals:
    void sendTitles(QStringList &titles);

private slots:
    void addRecord();
    void delRecord();
    void applyRecord();
    void cancelRecord();
    void selectCargo();
    void readRecord();
    void saveRecord();

public slots:
    void complete(int index);

};

#endif // INVENTORYWIDGET_H
