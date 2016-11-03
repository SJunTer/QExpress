#include "deliverypath.h"
#include <QDebug>

DeliveryPath::DeliveryPath()
    : vehicle("")
    , cargo("")
    , pos(0)
    , moveSec(0)
{
}

void DeliveryPath::setData(QVector<long> &points, QVector<long> &path, QStringList &nameList)
{
    s_path s_pa;
    for(auto n : path)
    {
        s_pa.index = n;
        s_pa.time = QTime();
        m_path.append(s_pa);
    }
    s_point s_pt;
    for(int i = 0; i < points.size(); ++i)
    {
        s_pt.index = points[i];
        s_pt.name = nameList[i];
        m_points.append(s_pt);
    }
}

void DeliveryPath::updateStatus()
{
    if(QTime::currentTime() >= m_path.at(pos).time)
    {
        if(pos == 0)
        {
            startTime = QTime::currentTime().toString("hh:mm:ss");
            status = "车辆已出库";
            ++pos;
        }
        else if(pos == m_path.size()-1)
        {
            status = "配送完成";
            ++pos;
            timer->stop();
        }
        else
        {
            for(auto p : m_points)
            {
                if(p.index == m_path.at(pos).index)
                {
                    status = QString("车辆已到达%1").arg(p.name);
                    break;
                }
            }
            ++pos;
        }
        emit posChange(pos, m_index);
    }
}

//通过随机函数生成时间表
void DeliveryPath::randomSchedule(int index)
{
    m_index = index;
    QTime time = QTime::currentTime();
    for(int i = 0; i < m_path.size(); ++i)
    {
        time = time.addSecs(qrand()%1+1);
        m_path[i].time = time;
    }
    //启动计时器
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatus()));
    timer->start(1000);
    status = "未出发";
}

void DeliveryPath::clear()
{
    vehicle = "";
    cargo = "";
    m_path.clear();
    m_points.clear();
}
