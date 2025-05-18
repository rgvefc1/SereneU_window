#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QHBoxLayout>

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ButtonDelegate(QObject* parent = nullptr);
    ~ButtonDelegate() override = default;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

signals:
    void completeButtonClicked(const QModelIndex& index) const;   // 수정 버튼 클릭 시그널
    void deleteButtonClicked(const QModelIndex& index) const; // 삭제 버튼 클릭 시그널
    void buttonClicked(const QModelIndex& index);
};

#endif // BUTTONDELEGATE_H
