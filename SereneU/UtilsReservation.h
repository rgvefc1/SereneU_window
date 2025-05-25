#include <qstring.h>
#include <QWidget>                // QWidget
#include <QMap>                   // QMap
#include <QtSql/QSqlDatabase>     // QSqlDatabase
#include <QtSql/QSqlQuery>        // QSqlQuery
#include <qtableview.h>
#include <qdatetime.h>

#ifndef UTILSRESERVATION_H
#define UTILSRESERVATION_H

// reservationData 구조체
struct ReservationData {
    int           reservationId;        // 예약ID
    QDateTime     createdAt;              // 예약 생성 시간
    QDateTime     updatedAt;              // 예약 수정 시간
    int           customerId;           // 예약자 ID
    QString       customerName;         // 예약자 이름
    QString       customerPhone;        // 예약자 전화번호
    int           serviceId;            // 시술 ID
    QString       serviceName;          // 시술 이름
    int           price;                // 시술 가격
    int           deposit;              // 예약금
    QDateTime     reservationTime;        // 예약 시간
    int           status;               // status: "0: 확정","1: 리터치", " 2.완료","3.취소","4.노쇼"
    bool          retouch;              // retouch
    QString       notes;                // 메모
    QString       cancellationReason;   // 예약 취소 사유.
};

class UtilsReservation
{

public:
    explicit UtilsReservation();
    ~UtilsReservation();

    bool searchReservationByDate(QTableView* tableview, const QString& date);
    bool searchReservationByCustomer(QTableView* tableview, int customerId);
    bool searchReservationAll(QTableView* tableview);
    bool insertReservation(ReservationData data);
    bool updateReservation(ReservationData data);
    bool deleteReservation(int reservationId);

private:
    QSqlDatabase db;
    QSqlQuery query;
};
#endif UTILSRESERVATION_H
