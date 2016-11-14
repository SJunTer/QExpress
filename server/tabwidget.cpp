#include "tabwidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QFont>

/* ***************************************** *
 * ----------------------------------------- *
 * |                TabBar                 | *
 * ----------------------------------------- *
 * ***************************************** */

TabBar::TabBar(QWidget *parent)
    : QWidget(parent)
{
    m_currentIndex = -1;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumWidth(1200);
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
}

// 判断索引是否正确
bool TabBar::validIndex(int index) const
{
    return index >= 0 && index < m_tabs.count();
}

// 返回当前标签索引
int TabBar::currentIndex() const
{
    return m_currentIndex;
}


// 设置选中标签。仅当标签可用时，重置索引并刷新界面
void TabBar::setCurrentIndex(int index)
{
    if(!isTabEnabled(index))
    {
        qDebug() << "isTabEnabled : unvalid index";
        return;
    }
    m_currentIndex = index;
    update();
    emit currentChanged(m_currentIndex);
}

// 判断指定标签是否可用
bool TabBar::isTabEnabled(int index) const
{
    if(!validIndex(index))
    {
    }
    return m_tabs.at(index)->enabled;
}

// 设置标签可用性，并刷新标签区域
void TabBar::setTabEnabled(int index, bool enable)
{
    if(!validIndex(index))
    {
        qDebug() << "setTabEnabled : unvalid index";
        return;
    }
    m_tabs.at(index)->enabled = enable;
    update(tabRect(index));
}

// 在指定位置插入标签
void TabBar::insertTab(int index, const QString &text, const QString &desp)
{
    TabItem *tab = new TabItem(this);
    tab->text = text;
    tab->desp = desp;
    m_tabs.insert(index, tab);
    updateGeometry();
}

// 从标签栏中移除标签并销毁对象，更新视图
void TabBar::removeTab(int index)
{
    TabItem *tab = m_tabs.takeAt(index);
    delete tab;
    updateGeometry();
}

// 重置建议窗口尺寸
QSize TabBar::sizeHint() const
{
    QSize size = tabSize();

    return QSize((size.width()+TAB_SPACING)*count(), size.height()+2*TAB_MARGIN);
}

// 返回标签默认尺寸
QSize TabBar::tabSize() const
{
    return QSize(TAB_WIDTH, TAB_HEIGHT);
}


// 返回标签的几何信息
QRect TabBar::tabRect(int index) const
{
    QSize size = tabSize();

    return QRect(LEFT_INDENT+ICON_SIZE+2*TAB_SPACING+index*(size.width()+TAB_SPACING),
                 TAB_MARGIN, size.width(), size.height());
}

// 绘制单个标签
void TabBar::paintTab(QPainter *painter, int index) const
{
    if(!validIndex(index))
    {
        qDebug() << "paintTab : unvalid index";
        return;
    }

    // 将painter压栈保护
    painter->save();

    QRect rect = tabRect(index);

    if(index == m_currentIndex) // 选中项
    {
        painter->fillRect(rect, Qt::gray);
    }
    else if(!m_tabs[index]->enabled) // 无法选中项
    {
        painter->fillRect(rect, Qt::gray);
    }
    else    // 正常项
    {
        painter->fillRect(rect, Qt::black);
    }
    QFont font;
    painter->setPen(Qt::white);
    font.setPixelSize(16);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(QRect(rect.left(),rect.top(),rect.width()/3,rect.height()),
                      Qt::AlignCenter, m_tabs[index]->text);
    font.setPixelSize(12);
    font.setBold(false);
    painter->setFont(font);
    painter->drawText(rect.left()+rect.width()/3,rect.top()+rect.height()*3/4,m_tabs[index]->desp);

    painter->restore();

}

// 重载绘图函数
void TabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    //Background color
    painter.fillRect(rect(), QColor(255,255,255,210));

    //draw icon
    QPixmap pic(":/images/info.png");
    painter.drawPixmap(LEFT_INDENT,(TAB_HEIGHT+2*TAB_MARGIN-ICON_SIZE)/2,
                        ICON_SIZE, ICON_SIZE,pic);

    for(int i = 0; i < m_tabs.count(); ++i)
            paintTab(&painter, i);
}

// 鼠标按下时设置标签并刷新窗口
void TabBar::mousePressEvent(QMouseEvent *event)
{
    for(int index = 0; index < m_tabs.count(); ++index)
    {
        if(tabRect(index).contains(event->pos()) &&
                m_tabs[index]->enabled)
        {
            m_currentIndex = index;
            update();
            emit currentChanged(m_currentIndex);
            break;
        }
    }
}




/* ***************************************** *
 * ----------------------------------------- *
 * |               TabWidget               | *
 * ----------------------------------------- *
 * ***************************************** */

TabWidget::TabWidget(QWidget *parent)
    : QWidget(parent)
{
    m_tabBar = new TabBar(this);
    m_modeStack = new QStackedLayout;

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(1);
    layout->setMargin(0);
    layout->addWidget(m_tabBar);
    layout->addLayout(m_modeStack);
    setLayout(layout);

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(showWidget(int)));
}

int TabWidget::currentIndex() const
{
    return m_tabBar->currentIndex();
}

void TabWidget::setCurrentIndex(int index)
{
    if (m_tabBar->isTabEnabled(index))
        m_tabBar->setCurrentIndex(index);
}

bool TabWidget::isTabEnabled(int index) const
{
    return m_tabBar->isTabEnabled(index);
}

void TabWidget::setTabEnabled(int index, bool enable)
{
    m_tabBar->setTabEnabled(index, enable);
}

void TabWidget::insertTab(int index, QWidget *widget, const QString &text, const QString &desp)
{
    //将widget插入m_modestack并插入标签
    m_modeStack->insertWidget(index, widget);
    m_tabBar->insertTab(index, text, desp);
}

void TabWidget::removeTab(int index)
{
    //从m_modeStack中移除widget并移除标签
    m_modeStack->removeWidget(m_modeStack->widget(index));
    m_tabBar->removeTab(index);
}

void TabWidget::showWidget(int index)
{
    m_modeStack->setCurrentIndex(index);
}
