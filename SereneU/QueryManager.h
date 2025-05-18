#ifndef QUERYMANAGER_H
#define QUERYMANAGER_H

#include <QtSql\QSqlQuery>
#include <QtSql\QSqlQueryModel>
#include <QString>
#include <qtableview.h>

class QueryManager
{
public:
    // 싱글톤 인스턴스
    static QueryManager& instance();

    bool fillReservationsByDate(const QString& date, QTableView* tableView);

    // 쿼리 실행 및 모델 반환
    //QSqlQueryModel* getReservationsByDate(const QString& date, QObject* parent = nullptr);
    bool insertReservation(int customerId, int serviceType, const QString& reservationTime, bool retouch, int deposit);
    //예약 수정
    bool updateReservation(int reservationId, QString customerId, int serviceType, const QString& reservationTime, bool retouch, int deposit);
    // 단일 결과 반환 (예: 고객 수)
    int getCustomerCount();
    // 전체 고객 정보 반환
    bool fillCustomerData(QTableView* tableView,QString name, QString phone);
private:
    QueryManager() = default;
    ~QueryManager() = default;

    QueryManager(const QueryManager&) = delete;
    QueryManager& operator=(const QueryManager&) = delete;
};

#endif // QUERYMANAGER_H
