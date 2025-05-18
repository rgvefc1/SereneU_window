#include "ButtonDelegate.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QApplication>
ButtonDelegate::ButtonDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

// 🔨 버튼 생성 (한 컬럼에 2개)
QWidget* ButtonDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QWidget* editor = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(editor);
    layout->setContentsMargins(0, 0, 0, 0);

    // ✏️ 수정 버튼
    QPushButton* editButton = new QPushButton("완료", editor);
    connect(editButton, &QPushButton::clicked, [index, this]() {
        emit completeButtonClicked(index);
        });

    // 🗑️ 삭제 버튼
    QPushButton* deleteButton = new QPushButton("삭제", editor);
    connect(deleteButton, &QPushButton::clicked, [index, this]() {
        emit deleteButtonClicked(index);
        });

    // 레이아웃에 버튼 추가
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);

    return editor;
}
void ButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index) const
{
    QStyleOptionButton button;
    button.rect = option.rect;
    button.text = "상세보기";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool ButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        emit buttonClicked(index);  // 버튼 클릭 이벤트 발생
        return true;
    }
    return false;
}