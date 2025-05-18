#include "AddCustomer.h"
#include "ui_AddCustomer.h"
#include <QtSql/qsqlquery.h>
#include<QMessageBox>
#include<QtSql\qsqlerror.h>
#include <ConfirmDialog.h>

AddCustomer::AddCustomer(int mode,QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddCustomer),
    mode(mode),
    listModel(new QStringListModel(this))  // ✅ 정확한 선언

{
    ui->setupUi(this);
    setWindowTitle("고객 추가");
    setupUI();
    // QListView에 모델 연결
    ui->listView->setModel(listModel);
    // QListView 수정 불가능, 선택만 가능
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 수정 불가능
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection); // 단일 선택 가능

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
// 전화번호 중복 여부 검사
bool AddCustomer::isPhoneNumberExists(const QString& phoneNumber)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT COUNT(*) FROM "CUSTOMER"
        WHERE "CUSTOMER_PHONE" = :phone
    )");
    query.bindValue(":phone", phoneNumber);

    if (!query.exec()) {
        return true;  // 실패 시 중복된 것으로 처리
    }

    query.next();
    int count = query.value(0).toInt();
    return count > 0;  // 1 이상이면 중복
}

// 고객 추가
void AddCustomer::addCustomer(const QString& name, const QString& phoneNumber)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO "CUSTOMER" ("CUSTOMER_NAME", "CUSTOMER_PHONE")
        VALUES (:name, :phone)
    )");
    query.bindValue(":name", name);
    query.bindValue(":phone", phoneNumber);

    if (!query.exec()) {
        QMessageBox::critical(this, "오류", "고객 추가 실패: " + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "성공", "고객이 성공적으로 추가되었습니다.");
    accept();  // 다이얼로그 닫기
}

//고객 정보
QMap<QString, QString> AddCustomer::sellectCustomer(QString name, QString phone)
{
    if (phone.startsWith("0")) {
        phone.remove(0, 1);
    }
    phone = "%" + phone + "%";

    QMap<QString, QString> customerMap;
    QSqlQuery query;
    query.prepare(R"(
        SELECT "CUSTOMER"."CUSTOMER_PHONE","CUSTOMER"."CUSTOMER_NAME"
        FROM "CUSTOMER"
        WHERE (COALESCE(CAST(:phone AS TEXT), '') = '' OR "CUSTOMER_PHONE"::TEXT ILIKE :phone)
            AND (COALESCE(:name, '') = '' OR "CUSTOMER_NAME" ILIKE :name);
    )");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    

    if (query.exec()) {
        while (query.next()) {
            QString customerPhone = query.value("CUSTOMER_PHONE").toString();
            QString customerName = query.value("CUSTOMER_NAME").toString();
            customerMap.insert(customerPhone, customerName);
        }
    }
    else {
        qDebug() << "❌ 쿼리 실행 실패: " << query.lastError().text();
    }
    return customerMap;
}

// 저장 버튼 클릭 시
void AddCustomer::onSaveClicked()
{
    QString name = ui->customerName->text().trimmed();
    QString phone = ui->customerPhone->text().trimmed();
    if (mode == 0) {
        if (name.isEmpty() || phone.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "이름과 전화번호를 모두 입력하세요.");
            return;
        }

        // 전화번호 중복 검사
        if (isPhoneNumberExists(phone)) {
            QMessageBox::warning(this, "중복 오류", "이미 등록된 전화번호입니다.");
            return;
        }

        // 고객 추가
        addCustomer(name, phone);
    }
    else {
        QModelIndex index = ui->listView->currentIndex();
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
    QString name = ui->customerName->text().trimmed();
    QString phone = ui->customerPhone->text().trimmed();
    if (phone.isEmpty() && name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "이름과 전화번호 중 하나는 필수");
        return;
    }

    QMap<QString, QString> searchCustomer = sellectCustomer(name, phone);

    if (searchCustomer.isEmpty()) {
        if (mode == 1) {
            if (phone.isEmpty()) 
            {
                QMessageBox::information(this, "검색 결과", "일치하는 고객 정보가 없습니다.");
            }
            else {
                // 정규식 패턴 (전화번호 추출)
                QRegularExpression phonePattern(R"(\s*(\d{10,11}))");  // 10~11자리 숫자
                // 정규식 매칭
                QRegularExpressionMatch match = phonePattern.match(phone);
                int phoneNumber = match.captured(1).toInt();
                if (phoneNumber == 0) {
                    QMessageBox::warning(this, "입력 오류", "유효한 전화번호를 입력하세요.");
                    return;
                }
                else {
                    // 🛑 커스텀 확인 다이얼로그 표시
                    ConfirmDialog confirmDialog(this, QString("이름 : %1  전화번호 : %2 \n 저장되지 않은 고객입니다. \n 해당 정보로 고객을 등록하고 \n예약을 계속 하시겠습니까?").arg(name).arg(phone));
                    if (confirmDialog.exec() == QDialog::Accepted) {
                        // ✅ 사용자가 확인 버튼을 눌렀을 경우
                        emit onCustomerSelected(QString("이름 : %1  , 전화번호 : %2").arg(name).arg(phone));
                        addCustomer(name, phone);
                    }
                }
            }
        }
        else {
            QMessageBox::information(this, "검색 결과", "일치하는 고객 정보가 없습니다.");
        }
        
    }
    else {
        // QMap 데이터를 QStringList로 변환
        QStringList displayList;
        for (auto it = searchCustomer.begin(); it != searchCustomer.end(); ++it) {
            displayList.append(QString("이름 : %1  , 전화번호 : 0%2").arg(it.value()).arg(it.key()));
        }
        // 모델에 데이터 설정
        listModel->setStringList(displayList);
    }

    
}
