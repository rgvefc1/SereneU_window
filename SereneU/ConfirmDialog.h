#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QDialog>

namespace Ui {
    class ConfirmDialog;
}

class ConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfirmDialog(QWidget* parent = nullptr, const QString& message = "확인하시겠습니까?");
    ~ConfirmDialog();

private:
    Ui::ConfirmDialog* ui;
};

#endif // CONFIRMDIALOG_H
