#ifndef ADDRESERVATION_H
#define ADDRESERVATION_H

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QDate>

namespace Ui {
    class AddReservation;
}

class AddReservation : public QDialog
{
    Q_OBJECT

public:
    explicit AddReservation(QWidget* parent = nullptr, const QDate& selectedDate = QDate::currentDate());
    ~AddReservation();

    void populateServiceComboBox(QComboBox* comboBox);
    void onSelectCustomer();
signals:
    void reservationCompleted();  // 예약 완료 시 시그널

private:
    Ui::AddReservation* ui;
    QDate initialDate;            // 초기 선택된 날짜 저장
private slots:

    void onCustomerSelected(const QString& selectedCustomer);
    void onSaveClicked();// 예약 완료 처리
};

#endif // ADDRESERVATION_H
