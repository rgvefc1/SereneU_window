#include "ReservationDetail.h"
#include "ui_ReservationDetail.h"
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include <QueryManager.h>
#include <AddCustomer.h>
#include <ServiceTypeManager.h>

ReservationDetail::ReservationDetail(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ReservationDetail)
{
    ui->setupUi(this);
    setWindowTitle("예약 상세 보기");

    // 콤보박스에 서비스 목록 채우기
    QMap<int, QString> serviceType = ServiceTypeManager::instance().getAllServiceTypes();
    for (auto it = serviceType.constBegin(); it != serviceType.constEnd(); ++it) {
        int id = it.key();
        QString name = it.value();
        ui->serviceType->addItem(name, id);
    }
    
    // 버튼 클릭 시그널 연결
    connect(ui->selectCustomer, &QPushButton::clicked, this, &ReservationDetail::onSelectCustomer);

    connect(ui->editBtn, &QPushButton::clicked, this, &ReservationDetail::onModifyClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ReservationDetail::onDeleteClicked);



}

// ✅ 예약 정보를 UI에 반영
void ReservationDetail::setReservationData(const int reservationId,const QString& name, const QString& phone, const QString& reservationDate,
    const QString& serviceType, int deposit, bool retouch)
{
    this->reservationId = reservationId;
    ui->selectedCustomer->setText(QString("이름 : %1   전화번호 : %2").arg(name, phone));
    QDateTime dt = QDateTime::fromString(reservationDate,"yyyy-MM-dd HH시mm분");
    ui->dateEdit->setDate(dt.date());
    ui->timeEdit->setTime(dt.time());
    /* 4) 기타 필드 */
    ui->serviceType->setCurrentText(serviceType);
    ui->deposit->setValue(deposit);           // 스핀박스에 값 입력
    ui->retouch->setChecked(retouch);
}


// ✅ 수정 버튼 클릭 시
void ReservationDetail::onModifyClicked()
{

    QDate  date = ui->dateEdit->date();            
    QTime  time = ui->timeEdit->time();           
    QDateTime dt(date, time);
    QString reservationTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    QString customerId = ui->selectedCustomer->text();
    customerId.remove("-");
    customerId = customerId.right(11);
    
    int serviceType = ui->serviceType->currentIndex();
    bool retouch = ui->retouch->isChecked();
    int deposit = ui->deposit->value();

    if (QueryManager::instance().updateReservation(this->reservationId, customerId, serviceType, reservationTime, retouch, deposit)) {
        QMessageBox::information(this, "완료", "예약 수정이 완료되었습니다.");
        emit reservationUpdated();
        accept();
    }
    else {
        QMessageBox::information(this, "오류", "예약 수정이 실패했습니다.");
        accept();
    }

    //
}

// ✅ 삭제 버튼 클릭 시
void ReservationDetail::onDeleteClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "예약 삭제","정말 삭제하시겠습니까?",QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        emit reservationDeleted(reservationId);
        accept();
    }
}

void ReservationDetail::onCustomerSelected(const QString& customerInfo)
{
    ui->selectedCustomer->setText(customerInfo);
}

void ReservationDetail::onSelectCustomer()
{
    AddCustomer* addAddCustomerDialog = new AddCustomer(1, this);
    addAddCustomerDialog->setModal(true);  // 모달 모드
    connect(addAddCustomerDialog, &AddCustomer::onCustomerSelected, this, &ReservationDetail::onCustomerSelected);
    addAddCustomerDialog->exec();          // 모달로 창 띄우기
}

ReservationDetail::~ReservationDetail()
{
    delete ui;
}
