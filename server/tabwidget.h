#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QRect>
#include <QSize>
#include <QIcon>
#include <QPainter>


#define TAB_WIDTH 170
#define TAB_HEIGHT 33
#define TAB_SPACING 5
#define TAB_MARGIN 20
#define ICON_SIZE 48
#define LEFT_INDENT 30

/* ***************************************** *
 * ----------------------------------------- *
 * |                TabItem                | *
 * ----------------------------------------- *
 * ***************************************** */

class TabItem : public QObject
{
    Q_OBJECT
public:
    TabItem(QWidget *bar) : QObject(bar), enabled(false), tabBar(bar) {}

    QString text;
    QString desp;
    bool enabled;

private:
    QWidget *tabBar;
};



/* ***************************************** *
 * ----------------------------------------- *
 * |                TabBar                 | *
 * ----------------------------------------- *
 * ***************************************** */

class TabBar : public QWidget
{
    Q_OBJECT
public:
    TabBar(QWidget *parent = 0);

    bool validIndex(int index) const;
    int currentIndex() const;
    void setCurrentIndex(int index);

    int count() const { return m_tabs.count(); }

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool enable);

    void insertTab(int index, const QString &text, const QString &desp);
    void removeTab(int index);

    QSize sizeHint() const;
    QRect tabRect(int index) const;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    QSize tabSize() const;
    void paintTab(QPainter *painter, int index) const;

    int m_currentIndex;
    QList<TabItem *> m_tabs;

signals:
    void currentChanged(int index);


};


/* ***************************************** *
 * ----------------------------------------- *
 * |               TabWidget               | *
 * ----------------------------------------- *
 * ***************************************** */

class TabWidget : public QWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget *parent = 0);
    //初始化标签部件 void init();

    int currentIndex() const;
    void setCurrentIndex(int index);

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool enable);

    void insertTab(int index, QWidget *widget, const QString &text, const QString &desp);
    void removeTab(int index);

private:
    TabBar *m_tabBar;
    QStackedLayout *m_modeStack;

public slots:
    void showWidget(int index);

};

#endif // TABWIDGET_H
