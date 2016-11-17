#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QPushButton;
class QTableWidget;
QT_END_NAMESPACE


enum cargoStatus {
    InRepo, IsMove, IsComplete
};


class CargoInfo : public QObject
{
    Q_OBJECT
public:
    CargoInfo() : m_isSelect(false), m_index(0), m_descrip(""),
        m_dest(""), m_reciver(""), m_status(InRepo) {}

    bool m_isSelect;

    int m_index;
    QString m_descrip;
    QString m_dest;
    QString m_reciver;
    cargoStatus m_status;

public slots:
    // 响应复选框
    void stateChanged(int flag) { m_isSelect = flag; }
};


class InventoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InventoryWidget(QWidget *parent = 0);
    ~InventoryWidget();

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

    CargoInfo *temp;
    QList<CargoInfo *> cargos;

    QStringList titles;

    bool selectMode;
    bool editMode;


signals:
    void sendTitles(QStringList &titles);

private slots:
    void addRecord();
    void delRecord();
    void applyRecord();
    void cancelRecord();
    void selectCargo();

public slots:
    void complete(int index);

};

#endif // INVENTORYWIDGET_H
