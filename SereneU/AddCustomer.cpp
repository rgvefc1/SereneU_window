#include "AddCustomer.h"
#include "ui_AddCustomer.h"
#include <QtSql/qsqlquery.h>
#include<QMessageBox>
#include<QtSql\qsqlerror.h>

#include <UtilsCustomer.h>
#include <ConfirmDialog.h>
#include <UtilsCustomer.h>

AddCustomer::AddCustomer(int mode,QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddCustomer),
    mode(mode)
    //listModel(new QStringListModel(this))  // ✅ 정확한 선언

{
    ui->setupUi(this);
    setWindowTitle("고객 추가");
    setupUI();
    // QListView에 모델 연결
    //ui->tableView->setModel(listModel);
    // QListView 수정 불가능, 선택만 가능
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 수정 불가능
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // 단일 선택 가능

    // 저장 버튼 연결
    connect(ui->saveBtn, &QPushButton::clicked, this, &AddCustomer::onSaveClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->searchCustomer, &QPushButton::clicked, this, &AddCustomer::onSearchClicked);
}

AddCustomer::~AddCustomer()
{
    delete ui;
}

// UI 초기 설정 (모드에 따라 설정)
void AddCustomer::setupUI()
{
    if (mode == 0) {
        ui->saveBtn->setText("등록");  // 등록 모드
        setWindowTitle("고객등록");
    }
    else {
        ui->saveBtn->setText("선택");  // 생성 모드
        setWindowTitle("고객선택");
    }
}

// 고객 추가
void AddCustomer::addCustomer(const QString& name, const QString& phoneNumber)
{
    UtilsCustomer util;
    int result = util.insertCustomer(this,name,phoneNumber);
    accept();  // 다이얼로그 닫기
}

// 저장 버튼 클릭 시
void AddCustomer::onSaveClicked()
{
    UtilsCustomer util;
    QString name = ui->customerName->text().trimmed();
    QString phone = ui->customerPhone->text().trimmed();
    if (mode == 0) {
        if (name.isEmpty() || phone.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "이름과 전화번호를 모두 입력하세요.");
            return;
        }

        // 전화번호 중복 검사
        if (util.isPhoneNumberExists(phone)) {
            QMessageBox::warning(this, "중복 오류", "이미 등록된 전화번호입니다.");
            return;
        }

        // 고객 추가
        addCustomer(name, phone);
    }
    else {
        QModelIndex index = ui->tableView->currentIndex();
        if (index.isValid()) {
            QString selectedText = index.data().toString();
            emit onCustomerSelected(selectedText);  // 시그널로 데이터 전달
            accept();  // 다이얼로그 닫기
        }
        else {
            QMessageBox::warning(this, "입력 오류", "선택된 항목이 없습니다.");
        }
    }
    
}
//조회버튼 클릭
void AddCustomer::onSearchClicked()
{
    UtilsCustomer util;

    QString name = ui->customerName->text().trimmed();
    QString phone = ui->customerPhone->text().trimmed();
    if (phone.isEmpty() && name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "이름과 전화번호 중 하나는 필수");
        return;
    }

    bool result = util.searchCustomerInfo(ui->tableView,name, phone);

    if (!result) {
        // 검색된 고객이 하나도 없으면 여기로
        if (mode == 1 && !phone.isEmpty()) {
            // 예약 흐름: 전화번호가 있으면 신규 등록 유도
            ConfirmDialog confirm(this,
                tr("이름: %1\n전화번호: %2\n저장되지 않은 고객입니다.\n등록하고 계속하시겠습니까?")
                .arg(name).arg(phone));
            if (confirm.exec() == QDialog::Accepted) {
                emit onCustomerSelected(
                    tr("이름: %1, 전화번호: %2").arg(name).arg(phone));
                addCustomer(name, phone);
            }
        }
        else {
            // 모드 0(선택) 이거나 전화번호 미입력 등
            QMessageBox::information(this,
                tr("검색 결과"),
                tr("일치하는 고객 정보가 없습니다."));
        }
    }

    
}
