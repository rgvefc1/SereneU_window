#include "AddReservation.h"
#include <ui_AddReservation.h>
#include <ServiceTypeManager.h>
#include <QuaryManager.h>
#include <SereneuUtils.h>
#include <QMessageBox>

AddReservation::AddReservation(QWidget* parent, const QDate& selectedDate) :
    QDialog(parent),
    ui(new Ui::AddReservation),
    initialDate(selectedDate)  // 전달된 날짜 저장
{
    ui->setupUi(this);
    setWindowTitle("예약 추가");

    // 📅 DateEdit에 선택된 날짜 자동 설정
    ui->dateEdit->setDate(initialDate);
    QTime currentTime = QTime::currentTime();     // 현재 시간
    ui->timeEdit->setTime(currentTime);

    // 콤보박스에 서비스 목록 채우기
    populateServiceComboBox(ui->serviceType);

   

    // 저장 버튼 연결
    connect(ui->saveBtn, &QPushButton::clicked, this, &AddReservation::onSaveClicked);
    connect(ui->cancleBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->selectCustomer, &QPushButton::clicked, this, &AddReservation::onSelectCustomer);
    //connect(ui->searchCustomer, &QPushButton::clicked, this, &AddCustomer::onSearchClicked);

}

AddReservation::~AddReservation()
{
    delete ui;
}
// 서비스 콤보박스에 이름과 ID 연결
void AddReservation::populateServiceComboBox(QComboBox* comboBox)
{
    if (!comboBox)     // null 보호
        return;

    /* 1) 서비스 목록 가져오기 */
    const auto serviceTypes = ServiceTypeManager::instance().getAllServiceTypes();

    // 기존 항목 초기화
    comboBox->clear();

    // 서비스 이름과 ID 연결
    for (auto it = serviceTypes.cbegin(); it != serviceTypes.cend(); ++it) {
        int     serviceId = it.key();
        QString serviceName = it.value();
        comboBox->addItem(serviceName, serviceId);
    }

    // 첫 번째 항목 기본 선택
    if (ui->serviceType->count() > 0) {
        ui->serviceType->setCurrentIndex(0);
    }
}

void AddReservation::onCustomerSelected(const QString& customerInfo)
{
    ui->selectedCustomer->setText(customerInfo);
}

void AddReservation::onSaveClicked()
{
    //ReservationData data;
    ////전화번호
    //data.customerPhone = ui->selectedCustomer->text();
    //QString customer = ui->selectedCustomer->text();
    //// 정규식 패턴 (전화번호 추출)
    //QRegularExpression phonePattern(R"(전화번호\s*:\s*(\d{10,11}))");  // 10~11자리 숫자
    //// 정규식 매칭
    //QRegularExpressionMatch match = phonePattern.match(customer);
    //int phoneNumber = match.captured(1).toInt();

    ////시술종목
    //int serviceType = ui->serviceType->currentData().toInt();
    ////예약시간
    //QString reservationTime = SereneuUtils::createTimestamp(ui->dateEdit, ui->timeEdit);
    ////리터치
    //bool retouch = ui->retouch->isChecked();
    ////예약금
    //int deposit = ui->deposit->value();

    //// ❗ 빈값 검증 (전화번호, 시술종류, 예약시간 필수)
    //if (customer.isEmpty()) {
    //    QMessageBox::warning(this, "입력 오류", "고객 정보를 선택하세요.");
    //    return;
    //}

    //if (phoneNumber == 0) {
    //    QMessageBox::warning(this, "입력 오류", "유효한 전화번호를 입력하세요.");
    //    return;
    //}

    //if (serviceType == 0) {
    //    QMessageBox::warning(this, "입력 오류", "시술 종류를 선택하세요.");
    //    return;
    //}

    //if (reservationTime.isEmpty()) {
    //    QMessageBox::warning(this, "입력 오류", "예약 시간을 선택하세요.");
    //    return;
    //}
    //// 예약 등록
    //reservation.insertReservation();
    //if (QueryManager::instance().insertReservation(phoneNumber, serviceType, reservationTime, retouch, deposit)) {
    //    QMessageBox::information(this, "성공", "예약이 성공적으로 등록되었습니다.");
    //    // 🔔 예약 완료 시 시그널 발생
    //    emit reservationCompleted();
    //    accept();
    //}
    //else {
    //    QMessageBox::critical(this, "실패", "예약 등록에 실패했습니다.");
    //}

}

void AddReservation::onSelectCustomer()
{
    //AddCustomer* addAddCustomerDialog = new AddCustomer(1,this);
    //addAddCustomerDialog->setModal(true);  // 모달 모드
    //// 시그널-슬롯 연결
    //connect(addAddCustomerDialog, &AddCustomer::onCustomerSelected, this, &AddReservation::onCustomerSelected);
    //addAddCustomerDialog->exec();          // 모달로 창 띄우기
}