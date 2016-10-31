#ifndef DELIVERYPATH_H
#define DELIVERYPATH_H

#include <QObject>
#include <QString>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QTime>
#include <QTimer>

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

    QString vehicle;
    QString cargo;
    long pos;  // 当前经过点
    int moveSec;// 运行时间
    QString startTime;
    QString status;
    QVector<s_point> m_points; // 标记点
    QVector<s_path> m_path; // 详细路径途径点
    int m_index;

private:
    QTimer *timer; // 计时器

signals:
    void posChange(int pos, int index);

/*暂时先做成内部槽响应计时器*/
private slots:
    void updateStatus();

};

#endif // DELIVERYPATH_H
