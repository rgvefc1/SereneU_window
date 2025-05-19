#include "QueryManager.h"
#include "DBManager.h"
#include <QtSql\QSqlError>
#include <QDebug>
#include <QTableWidgetItem>
#include <QtSql\QSqlRecord>
#include <ReservationModel.h>
#include <customerModel.h>

// 싱글톤 인스턴스
QueryManager& QueryManager::instance()
{
    static QueryManager instance;
    return instance;
}




bool QueryManager::insertReservation(int customerId, int serviceType, const QString& reservationTime, bool retouch, int deposit)
{
    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(R"(
        INSERT INTO "RESERVATION" 
        ("CUSTOMER_ID", "SERVICE_TYPE", "RESERVATION_TIME", "RETOUCH", "DEPOSIT")
        VALUES (:customer_id, :service_type, :reservation_time, :retouch, :deposit)
    )");

    // 파라미터 바인딩
    query.bindValue(":customer_id", customerId);
    query.bindValue(":service_type", serviceType);
    query.bindValue(":reservation_time", reservationTime);
    query.bindValue(":retouch", retouch);
    query.bindValue(":deposit", deposit);

    // 쿼리 실행
    if (query.exec()) {
        return true;
    }
    else {
        return false;
    }
}

bool QueryManager::updateReservation(int reservationId, QString customerId, int serviceType, const QString& reservationTime, bool retouch, int deposit)
{
    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(R"(
        UPDATE "RESERVATION" 
        SET "RESERVATION_TIME" = :reservation_time,
            "CUSTOMER_ID" = :customer_id,
            "SERVICE_TYPE" = :service_type, 
            "RETOUCH" = :retouch, 
            "DEPOSIT" = :deposit
        WHERE "RESERVATION_ID" = :reservation_id
    )");
    // 파라미터 바인딩
    query.bindValue(":reservation_time", reservationTime);
    query.bindValue(":customer_id", customerId);
    query.bindValue(":service_type", serviceType);
    query.bindValue(":retouch", retouch);
    query.bindValue(":deposit", deposit);
    query.bindValue(":reservation_id", reservationId);

    // 쿼리 실행
    if(!query.exec()) {
        qWarning() << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        qWarning() << "[updateReservation] 조건에 맞는 행이 없습니다!"
            << "reservationId =" << reservationId;
        return false;
    }
    DBManager::instance().getDatabase().commit();
    return true;
}

// 고객 수 조회
int QueryManager::getCustomerCount()
{
    QSqlQuery query(DBManager::instance().getDatabase());
    query.exec("SELECT COUNT(*) FROM customers");

    if (query.next()) {
        return query.value(0).toInt();
    }

    qDebug() << "❌ 고객 수 조회 실패: " << query.lastError().text();
    return 0;
}
