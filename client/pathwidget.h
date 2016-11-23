#ifndef PATHWIDGET_H
#define PATHWIDGET_H

#include <QWidget>
#include <QList>
#include "deliverypath.h"

class PathWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PathWidget(QWidget *parent = 0);

private:
    QList<Place> places;

signals:

public slots:
};

#endif // PATHWIDGET_H
