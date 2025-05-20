#ifndef RESERVATIONDETAIL_H
#define RESERVATIONDETAIL_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

namespace Ui {
    class ReservationDetail;
}

class ReservationDetail : public QDialog
{
    Q_OBJECT

public:
    explicit ReservationDetail(QWidget* parent = nullptr);
    void setReservationData(const int reservationId,const QString& name, const QString& phone, const QString& reservationDate,
        const QString& serviceType, int deposit, bool retouch);
    ~ReservationDetail();
    void onSelectCustomer();
signals:
    void reservationUpdated();  // 예약 수정 후 새로고침을 위한 시그널
    void reservationDeleted(int reservationId);  // 예약 삭제 시그널

private slots:
    //void onCompleteClicked();  // 완료 버튼 클릭
    void onModifyClicked();  // 수정 버튼 클릭
    void onDeleteClicked();  // 삭제 버튼 클릭
    void onCustomerSelected(const QString& selectedCustomer);
private:
    Ui::ReservationDetail* ui;
    int reservationId;  // 예약 ID 저장
};

#endif // RESERVATIONDETAIL_H
