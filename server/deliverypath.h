#ifndef DELIVERYPATH_H
#define DELIVERYPATH_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QTime>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

enum PointType {
    IsRepo, IsDely, IsPass
};

// 途经点
struct point
{
    long index;
    QString name;
    PointType type;
    QTime time;
};

struct s_path
{
    long index;
    QTime time;
};

struct s_point
{
    long index;
    QString name;
};

class DeliveryPath : public QObject
{
    Q_OBJECT
public:
    DeliveryPath();

    //模拟行车时间函数（替代定位）
    void randomSchedule(int index);
    void clear();
    void setData(QVector<long> &points, QVector<long> &path, QStringList &nameList);

    int m_index;
    QString m_truck;
    QString m_cargo;
    QString m_status;
    QVector<s_point> m_points; // 配送点
    QVector<s_path> m_path; // 途径点
    QString startTime;  // 出发时间
    int moveTime;    // 运行时间
    long m_pos;     // 当前经过点

private:
    QTimer *timer; // 计时器

signals:
    void posChange(int m_pos, int index);

/*暂时先做成内部槽响应计时器*/
private slots:
    void updateStatus();

};

#endif // DELIVERYPATH_H
