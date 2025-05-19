#include "InsertCustomer.h"
#include "ui_InsertCustomer.h"
#include <UtilsCustomer.h>
#include <QMessageBox>


InsertCustomer::InsertCustomer(QWidget* parent)
	:QDialog(parent),
	ui(new Ui::insertCustomer)
{
    ui->setupUi(this);
    setWindowTitle("고객 추가");
    setupUI();
    connect(ui->saveBtn, &QPushButton::clicked, this, &InsertCustomer::onSaveBtnClick);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

InsertCustomer::~InsertCustomer()
{
	delete ui;
}

void InsertCustomer::onSaveBtnClick()
{
	UtilsCustomer util;
    CustomerData data;
	data.customerName = ui->customerName->text().trimmed();
	data.customerPhone = ui->customerPhone->text().trimmed();
    data.birthDate = ui->birthDate->date();
    ui->gender0->isChecked() ? data.gender = 0 : data.gender = 1;
    data.address = ui->address->text().trimmed();
    data.memo = ui->memo->toPlainText();

    if (data.customerName.isEmpty() || data.customerPhone.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "이름과 전화번호를 모두 입력하세요.");
        return;
    }

    // 전화번호 중복 검사
    if (util.isPhoneNumberExists(data.customerPhone)) {
        QMessageBox::warning(this, "중복 오류", "이미 등록된 전화번호 입니다.");
        return;
    }

    // 고객 추가
    QString result = util.insertCustomer(data);
    if (result == "success") {
        QMessageBox::information(this, "성공", "고객이 성공적으로 추가되었습니다.");
        accept();
    }
    else {
        QMessageBox::critical(this, "오류", "고객 추가 실패: " + result);
    }
}

void InsertCustomer::setupUI()
{
}
