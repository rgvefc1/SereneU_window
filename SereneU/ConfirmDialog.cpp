#include "ConfirmDialog.h"
#include "ui_ConfirmDialog.h"

ConfirmDialog::ConfirmDialog(QWidget* parent, const QString& message) :
    QDialog(parent),
    ui(new Ui::ConfirmDialog)
{
    ui->setupUi(this);
    setWindowTitle("확인");

    // 메시지 설정
   
    ui->messageLabel->setText(message);

    // 버튼 클릭 시 동작
    connect(ui->confirmButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);

}

ConfirmDialog::~ConfirmDialog()
{
    delete ui;
}
