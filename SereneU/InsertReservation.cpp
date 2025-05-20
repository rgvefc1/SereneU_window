#include "InsertReservation.h"
#include "ui_insertReservation.h"

#include <ServiceTypeManager.h>
#include <InsertCustomer.h>
#include <QRegularExpression>
#include <QPointer>
#include <UtilsReservation.h>
#include <QDateTime>
#include <QMessageBox>

/* -----------------------------------------------------
 *  정적 싱글톤 포인터
 *  파괴 시 QObject 출처에서 자동으로 nullptr 로 바뀜(QPointer)
 * ---------------------------------------------------*/
static QPointer<InsertReservation> s_instance;

/* ===== 정적 헬퍼 ===== */
InsertReservation* InsertReservation::showDialog(QWidget* parent, const QDate& date)
{
    if (!s_instance) {
        s_instance = new InsertReservation(parent, date);
        s_instance->setAttribute(Qt::WA_DeleteOnClose);   // 닫힘 시 자동 소멸
    }
    s_instance->show();
    s_instance->raise();
    s_instance->activateWindow();
    return s_instance;
}

/* ===== 생성 / 소멸 ===== */
InsertReservation::InsertReservation(QWidget* parent, const QDate& selectedDate)
    : QDialog(parent),
    ui(new Ui::InsertReservation),
    initialDate(selectedDate)
{
    ui->setupUi(this);
    setWindowTitle(tr("예약 등록"));

    // 기본 날짜·시간 세팅
    ui->reservationDate->setDate(initialDate);
    ui->reservationTime->setTime(QTime::currentTime());

    serviceComboBoxSetting(ui->serviceType);

    // 버튼 시그널 연결
    connect(ui->insertBtn, &QPushButton::clicked,this, &InsertReservation::insertReservationBtnClick);
    connect(ui->insertCustomerBtn, &QPushButton::clicked,this, &InsertReservation::insertCustomerBtnClick);
    connect(ui->updateBtn, &QPushButton::clicked,this, &InsertReservation::updateReservationBtnClick);
    connect(ui->cancelBtn, &QPushButton::clicked,this, &QDialog::reject);
    connect(ui->searchBtn, &QPushButton::clicked, this, &InsertReservation::searchBtnClick);
}

InsertReservation::~InsertReservation()
{
    delete ui;
    s_instance.clear();       // 🔹 다이얼로그가 지워질 때 전역 포인터도 null
}

/* ===== 버튼 슬롯 ===== */
void InsertReservation::insertCustomerBtnClick()
{
    auto* customerDialog = new InsertCustomer(this);
    connect(customerDialog, &InsertCustomer::insertCustomerSuccess,this, &InsertReservation::onCustomerSelected,Qt::UniqueConnection);
    customerDialog->exec();                     // 모달
    customerDialog->deleteLater();
}

void InsertReservation::searchBtnClick()
{
    const QString keyword = ui->searchData->text().trimmed();
    QString name, phone;

    static const QRegularExpression re(QStringLiteral("^\\d+$"));
    if (re.match(keyword).hasMatch())
        phone = keyword;          // 숫자 → 전화번호
    else
        name = keyword;          // 그 외 → 이름

    customer.searchCustomerInfo(ui->searchCustomerView, name, phone);

    ui->searchCustomerView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->searchCustomerView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->searchCustomerView->setColumnHidden(0, true);

    auto* sel = ui->searchCustomerView->selectionModel();
    if (sel) {
        connect(sel, &QItemSelectionModel::currentRowChanged,this, &InsertReservation::onCurrentRowChanged,Qt::UniqueConnection);          // 중복 연결 방지
    }
}

void InsertReservation::insertReservationBtnClick()
{
    data.serviceId = ui->serviceType->currentData().toInt();
    data.serviceName = ui->serviceType->currentText();
    QDate     d = ui->reservationDate->date();   // QDateEdit → QDate
    QTime     t = ui->reservationTime->time();   // QTimeEdit → QTime
    QDateTime dt(d, t);                           

    data.reservationTime = dt;                    // 구조체에 저장
    data.price = ui->price->value();
    data.deposit = ui->deposit->value();
    data.notes = ui->memo->toPlainText();

    if (ui->state_0->isChecked()) {data.status = 0;}
    else if (ui->state_1->isChecked()) {data.status = 1; }
    else if (ui->state_2->isChecked()) {data.status = 2; }
    else if (ui->state_3->isChecked()) {data.status = 3; }
    else if (ui->state_4->isChecked()) {data.status = 4; }

    int result = reservation.insertReservation(data);
    if (result) {
        QMessageBox::warning(this, "오류", "예약 등록에 실패했습니다.");
    }
    else {
        QString msg = QString("%1님의 %2<br>%3 시술 예약이 완료되었습니다.")
            .arg(data.customerName)    // %1
            .arg(data.reservationTime.toString("yyyy-MM-dd HH:mm:ss"))     // %2 (예: "2025-06-01 14:00")
            .arg(data.serviceName);    // %3
        QMessageBox::information(this,tr("예약 완료"),msg);     
    }
    accept();
    emit reservationCompleted();
}

void InsertReservation::updateReservationBtnClick()
{
    // TODO: 구현 필요
}

void InsertReservation::onCurrentRowChanged(const QModelIndex& current,const QModelIndex&/*previous*/)
{
  
    int row = current.row();            // 선택된 행 번호

    // ── ① 고객번호 ───────────────────────────────
    int customerId = current.sibling(row, 0)     // Qt < 6
        .data(Qt::DisplayRole)
        .toInt();
    data.customerId = customerId;

    // ── ② 고객이름 ────────────────────────────────
    QString custName = current.sibling(row, 1) // or .siblingAtColumn(ColName) in Qt6
        .data(Qt::DisplayRole)
        .toString();
    data.customerName = custName;

    // ── ③ 전화번호 ────────────────────────────────
    QString phone = current.sibling(row, 2)
        .data(Qt::DisplayRole)
        .toString();
    data.customerPhone = phone;
}

/* ===== 외부에서 호출되는 슬롯 ===== */
void InsertReservation::onCustomerSelected(const QString& name, const QString& phone)
{
    Q_UNUSED(name);
    ui->searchData->setText(phone);
    searchBtnClick();
}

/* ===== 내부 헬퍼 ===== */
void InsertReservation::serviceComboBoxSetting(QComboBox* comboBox)
{
    if (!comboBox) return;

    const auto serviceTypes = ServiceTypeManager::instance().getAllServiceTypes();
    comboBox->clear();

    for (auto it = serviceTypes.cbegin(); it != serviceTypes.cend(); ++it)
        comboBox->addItem(it.value(), it.key());

    if (comboBox->count() > 0)
        comboBox->setCurrentIndex(0);
}
