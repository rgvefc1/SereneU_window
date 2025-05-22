#include "SereneU.h"
#include "ui_SereneU.h"

#include "DBManager.h"
#include <QStandardItemModel>
#include <QuaryManager.h>
#include <CenterDelegate.h>
#include <QPushButton>
#include <ServiceTypeManager.h>
#include <QMessageBox>
#include <ButtonDelegate.h>
#include <ReservationDetail.h>
#include <DBConnector.h>
#include <ReservationModel.h>
#include <QStringListModel>
#include <customerModel.h>
#include <CustomerDetail.h>
#include <UtilsCustomer.h>
#include <UtilsReservation.h>
#include <InsertCustomer.h>
#include <InsertReservation.h>

SereneU::SereneU(QWidget* parent)
    :   QMainWindow(parent),
        ui(new Ui::SereneUClass)
{
    ui->setupUi(this);
    config = DBManager::instance().loadDBConfig("../config/dbConfig.xml");

    // DB 연결
    if (DBManager::instance().connectToDatabase(config.host,config.port,config.database,config.user,config.password)) {
        ui->dbConnect->setText("✅ DB 연결 성공!");
    }
    else {
        ui->dbConnect->setText("❌ DB 연결 실패!");
        return;
    }
    // 서비스 유형 데이터 로드
    ServiceTypeManager::instance().loadServiceTypes();

    // 프로그램 시작 시 오늘 날짜 조회
    QDate today = QDate::currentDate();
    ui->calendarWidget->setSelectedDate(today);  // 캘린더에서 오늘 날짜 선택
    onCalendarDateClicked(today);              // 예약 정보 조회

    // 📅 캘린더 날짜 선택 시
    connect(ui->calendarWidget, &QCalendarWidget::clicked,this, &SereneU::onCalendarDateClicked);
    // 📝 예약 추가 버튼 클릭 시
    connect(ui->addReservationButton, &QPushButton::clicked,this, &SereneU::onAddReservationButtonClicked);
    // 🔄 예약 완료 시 캘린더 새로고침
    connect(this, &SereneU::reservationUpdated,this, &SereneU::refreshCalendar);
    // QCalendarWidget 날짜 클릭 시 슬롯 연결
    connect(ui->addCustomerButton, &QPushButton::clicked, this, &SereneU::onOpenAddCustomerDialog);
    // 예약 완료 시 슬롯 연결
    connect(this, &SereneU::reservationUpdated, this, &SereneU::onCalendarDateClicked);
    //db연결 
    connect(ui->actionDB, &QAction::triggered, this, &SereneU::on_dbConnector_triggered);
    connect(ui->reservationView, &QTableView::doubleClicked, this, &SereneU::onReservationRowDoubleClicked);

    //고객관리 전체 고객 리스트 불러오기
    onCustomerView();
    //고객검색 기능
    connect(ui->searchCustomer_2, &QPushButton::clicked, this, &SereneU::onCustomerSearch);
    //고객 리스트 클릭
    connect(ui->customerView, &QTableView::doubleClicked, this, &SereneU::onCustormrRowDoubleClicked);
}

SereneU::~SereneU()
{
    DBManager::instance().closeConnection();
}

// 날짜 클릭 시 실행되는 함수
void SereneU::onCalendarDateClicked(const QDate& date)
{
    if (!date.isValid()) {
        qDebug() << "❌ 잘못된 날짜입니다.";
        return;
    }
    QString selectedDate = date.toString("yyyy-MM-dd");
    bool success = reservation.searchReservationByDate(ui->reservationView, selectedDate);

    if (!success) {
        qDebug() << "❌ 예약 정보 로딩 실패";
    }
    ui->reservationView->hideColumn(0);
    ui->reservationView->hideColumn(2);
    ui->reservationView->hideColumn(5);
    ui->reservationView->hideColumn(12);
}


void SereneU::applyCenterAlignment()
{
    CenterDelegate* centerDelegate = new CenterDelegate(this);
    ui->reservationView->setItemDelegate(centerDelegate);
}
//고객 추가 버튼 클릭
void SereneU::onOpenAddCustomerDialog()
{
    InsertCustomer* customer = new InsertCustomer();
    customer->setModal(true);  // 모달 모드
    customer->exec();          // 모달로 창 띄우기
}
//db연결 버튼 클릭
void SereneU::on_dbConnector_triggered()
{
    DBConnector* dbConnector = new DBConnector(this);
    dbConnector->exec();
}
//전체 고객정보 불러오기
void SereneU::onCustomerView()
{
    UtilsCustomer customer;
    customer.allCustomerInfo(ui->customerView);
    ui->customerView->hideColumn(0);
}
//고객정보 검색
void SereneU::onCustomerSearch()
{
    QString name = ui->customerName_2->text();
    QString phone = ui->customerPhone_2->text();
    bool result = customer.searchCustomerInfo(ui->customerView, name, phone);
    ui->customerView->hideColumn(0);
}
//고객리스트 더블 클릭 이벤트 처리
void SereneU::onCustormrRowDoubleClicked(const QModelIndex& index)
{
    int row = index.row();
    int col = index.column();
    CustomerData data;
    data.customerId = index.model()->data(index.model()->index(row, 0), Qt::DisplayRole).toInt();
    data.customerName = index.model()->data(index.model()->index(row, 1), Qt::DisplayRole).toString();
    data.customerPhone = index.model()->data(index.model()->index(row, 2), Qt::DisplayRole).toString();
    data.birthDate = index.model()->data(index.model()->index(row, 3), Qt::DisplayRole).toDate();
    data.gender = index.model()->data(index.model()->index(row, 4), Qt::DisplayRole).toBool();
    data.address = index.model()->data(index.model()->index(row, 5), Qt::DisplayRole).toString();
    data.visitRoot = index.model()->data(index.model()->index(row, 6), Qt::DisplayRole).toString();
    data.memo = index.model()->data(index.model()->index(row, 7), Qt::DisplayRole).toString();
    CustomerDetail customerDedail(data,this);
    customerDedail.exec();
}



// 📝 예약 추가 버튼 클릭 시 (선택된 날짜 전달)
void SereneU::onAddReservationButtonClicked()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    if (!selectedDate.isValid()) {
        QMessageBox::warning(this, "오류", "유효한 날짜를 선택하세요.");
        return;
    }
    InsertReservation* resDlg = InsertReservation::showDialog(this, selectedDate);
    connect(resDlg, &InsertReservation::reservationCompleted,this, &SereneU::refreshCalendar);  // 예약 완료 시 새로고침
    resDlg->clearReservationData(selectedDate);
}

// 🔄 캘린더 새로고침
void SereneU::refreshCalendar()
{
    QDate selectedDate = ui->calendarWidget->selectedDate();
    onCalendarDateClicked(selectedDate);
}

// ✅ 예약내역 버튼 클릭 이벤트 처리
void SereneU::onReservationRowDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        qDebug() << "❌ 유효하지 않은 인덱스입니다.";
        return;
    }
    QTableView* view = ui->reservationView;
    if (!view) {
        qDebug() << "❌ reservationView 포인터가 null입니다.";
        return;
    }
    int row = index.row();
    int col = index.column();

    // ✅ 선택된 예약 정보 가져오기
    ReservationData data;
    data.reservationId = index.model()->data(index.model()->index(row, 0), Qt::DisplayRole).toInt();
    QString timeStr = index.model()->data(index.model()->index(row, 1), Qt::DisplayRole).toString();
    QDateTime dt = QDateTime::fromString(timeStr, "yyyy-MM-dd HH시mm분");
    data.reservationTime = dt;
    data.customerId = index.model()->data(index.model()->index(row, 2), Qt::DisplayRole).toInt();
    data.customerName = index.model()->data(index.model()->index(row, 3), Qt::DisplayRole).toString();
    data.customerPhone = index.model()->data(index.model()->index(row, 4), Qt::DisplayRole).toString().replace("-","");
    data.serviceId= index.model()->data(index.model()->index(row, 5), Qt::DisplayRole).toInt();
    data.serviceName = index.model()->data(index.model()->index(row, 6), Qt::DisplayRole).toString();
    data.price = index.model()->data(index.model()->index(row, 7), Qt::DisplayRole).toInt();
    data.deposit = index.model()->data(index.model()->index(row, 8), Qt::DisplayRole).toInt();
    QString retouchStr = index.model()->data(index.model()->index(row, 9), Qt::DisplayRole).toString();
    data.retouch = (retouchStr == "✔️");
    QString statusStr = index.model()->data(index.model()->index(row, 10), Qt::DisplayRole).toString();
    if (statusStr == "예약") data.status = 0;
    else if (statusStr == "리터치") data.status = 1;
    else if (statusStr == "완료") data.status = 2;
    else if (statusStr == "취소") data.status = 3;
    else if (statusStr == "노쇼") data.status = 4;
    data.notes = index.model()->data(index.model()->index(row, 11), Qt::DisplayRole).toString();
    

    // ✅ 상세 보기 다이얼로그 생성 및 데이터 설정
    QDate dataTime = data.reservationTime.date();
    InsertReservation* resDlg = InsertReservation::showDialog(this, dataTime);
    resDlg->setReservationData(data);
    connect(resDlg, &InsertReservation::reservationCompleted, this, &SereneU::refreshCalendar);  // 예약 완료 시 새로고침

    

}