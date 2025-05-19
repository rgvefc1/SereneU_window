#ifndef SERENEU_H
#define SERENEU_H

#include <QMainWindow>
#include "ui_SereneU.h"
#include <DBManager.h>
#include <qstringlistmodel.h>
#include <UtilsReservation.h>
#include <UtilsCustomer.h>

class SereneU : public QMainWindow
{
    Q_OBJECT

public:
    SereneU(QWidget* parent = nullptr);
    ~SereneU();

    void applyCenterAlignment();
    DBConfig config;
private:
    Ui::SereneUClass* ui;
    UtilsReservation reservation;
    UtilsCustomer customer;
private slots:
    void onCalendarDateClicked(const QDate& date);  // 날짜 선택 시 호출
    void onAddReservationButtonClicked();           // 예약 추가 버튼 클릭 시
    void refreshCalendar();                         // 캘린더 새로고침
    void onReservationRowDoubleClicked(const QModelIndex& index);
    void onOpenAddCustomerDialog();
    void on_dbConnector_triggered();
    void onCustomerView();
    void onCustomerSearch();
    void onCustormrRowDoubleClicked(const QModelIndex& index);
signals:
    void reservationUpdated(const QDate& date);  // 예약 완료 시 날짜 전달
};

#endif // SERENEU_H
