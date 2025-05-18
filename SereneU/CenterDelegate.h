#ifndef CENTERDELEGATE_H
#define CENTERDELEGATE_H

#include <QStyledItemDelegate>

class CenterDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CenterDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    // 텍스트를 가운데 정렬
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);
        option->displayAlignment = Qt::AlignCenter;
    }
};

#endif // CENTERDELEGATE_H
