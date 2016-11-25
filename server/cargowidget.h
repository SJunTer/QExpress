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

struct CargoInfo
{
    int id;
    QString descrip;
    QString dest;
    QString reciver;
    int status;

};


class CargoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CargoWidget(Dbsql *d, QWidget *parent = 0);
    ~CargoWidget();

    void readInfo();

private:
    void initTable();

    Dbsql *dbsql;
    QTableWidget *cargoTable;
    QPushButton *addBtn;
    QPushButton *delBtn;
    QPushButton *applyBtn;

    CargoInfo *temp;
    QList<CargoInfo *> cargos;

    QStringList titles;

    bool editMode;

signals:
    void sendCargos(QList<CargoInfo*>*cs);

private slots:
    void addRecord();
    void delRecord();
    void applyRecord();

public slots:
    void cargoMove(QStringList cs);
    void cargoReach(QStringList cs);
    void cargofail(QStringList cs);

};

#endif // CARGOWIDGET_H
