#include "InsertCustomer.h"
#include "ui_InsertCustomer.h"
#include <UtilsCustomer.h>
#include <QMessageBox>
#include <InsertReservation.h>
#include <QPointer>

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
    data.visitRoot = ui->visitRoot->text();
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

    if (result != "success") {
        QMessageBox::critical(this, tr("오류"),
            tr("고객 추가 실패: %1").arg(result));
        return;
    }

    // 2) Yes/No 확인창
    auto reply = QMessageBox::question(
        this, tr("완료"),
        tr("고객이 등록되었습니다.\n등록된 고객으로 예약을 진행 하시겠습니까?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (reply == QMessageBox::No) {
        accept();                    // 창 닫고 끝
        return;
    }

    /* ───────────────────────────────────────────────
       3) InsertReservation 창이 이미 열려 있는지 확인
       ─────────────────────────────────────────────── */
    auto* resDlg = InsertReservation::showDialog(this);      // ✅ 한 줄

    connect(this, &InsertCustomer::insertCustomerSuccess,
        resDlg, &InsertReservation::onCustomerSelected,
        Qt::UniqueConnection);

    emit insertCustomerSuccess(data.customerName, data.customerPhone);

    accept();   // InsertCustomer 다이얼로그 닫기
}

void InsertCustomer::setupUI()
{
}