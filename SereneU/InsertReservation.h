#ifndef INSERTRESERVATION_H
#define INSERTRESERVATION_H

#include <QDialog>
#include <QDate>
#include <QComboBox>
#include <QPointer>
#include <UtilsReservation.h>
#include <UtilsCustomer.h>

namespace Ui { class InsertReservation; }

class InsertReservation : public QDialog
{
    Q_OBJECT
public:
    /**
     * 예약 다이얼로그를 재사용하거나 새로 생성해 반환한다.
     * - 이미 열린 창이 있으면 그 창을 활성화하고 반환
     * - 없으면 새로 만들어 show() 후 반환
     */
    static InsertReservation* showDialog(QWidget* parent = nullptr,
        const QDate& date = QDate::currentDate());

    ~InsertReservation() override;
    void setReservationData(ReservationData data);
    void clearReservationData(const QDate& date);
    explicit InsertReservation(QWidget* parent, const QDate& selectedDate);
signals:
    void reservationCompleted();                //!< 예약 완료 시그널

public slots:                                   // 슬롯 (외부에서 호출 가능)
    void onCustomerSelected(const QString& name, const QString& phone);

private slots:                                  // 내부 슬롯 (버튼 핸들러)
    void insertCustomerBtnClick();
    void searchBtnClick();
    void insertReservationBtnClick();
    void updateReservationBtnClick();
    void cancelBtnClick();
    void onCurrentRowChanged(const QModelIndex& current, const QModelIndex&/*previous*/);

private:                                        // 내부 헬퍼
    
    void serviceComboBoxSetting(QComboBox* comboBox);

    Ui::InsertReservation* ui = nullptr;
    QDate            initialDate;
    UtilsReservation reservation;
    UtilsCustomer    customer;
    ReservationData data;
};

#endif // INSERTRESERVATION_H