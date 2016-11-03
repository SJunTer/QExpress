#ifndef NOFOCUSDELEGATE_H
#define NOFOCUSDELEGATE_H

#include <QStyledItemDelegate>


/************************
 * 继承QStyledItemDelegate * 类
 * 用以消除选中虚框
 * **********************/
class NoFocusDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem itemOption(option);
        if (itemOption.state & QStyle::State_HasFocus)
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter, itemOption, index);
    }
};


#endif // NOFOCUSDELEGATE_H
