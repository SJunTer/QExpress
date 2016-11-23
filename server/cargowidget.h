#ifndef CARGOWIDGET_H
#define CARGOWIDGET_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QPushButton;
class QTableWidget;
class Dbsql;
QT_END_NAMESPACE


enum cargoStatus {
    InRepo, IsMove, IsComplete
};


class CargoInfo : public QObject
{
    Q_OBJECT
public:
    CargoInfo() : m_isSelect(false), id(0), descrip(""),
        dest(""), reciver(""), status(InRepo) {}

    bool m_isSelect;

    int id;
    QString descrip;
    QString dest;
    QString reciver;
    int status;

public slots:
    // 响应复选框
    void stateChanged(int flag) { m_isSelect = flag; }
};


class CargoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CargoWidget(Dbsql *d, QWidget *parent = 0);
    ~CargoWidget();

    void readInfo();

    void setSelectMode(bool mode) {
        selectMode = mode;
    }

private:
    void initTable();

    Dbsql *dbsql;
    QTableWidget *cargoTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *applyBtn;
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
    void selectCargo();

public slots:
    void complete(int i);

};

#endif // CARGOWIDGET_H
