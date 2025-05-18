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

bool QueryManager::fillReservationsByDate(const QString& date, QTableView* tableView)
{
    if (!DBManager::instance().isConnected()) {
        qDebug() << "❌ DB 연결이 안 되어 있습니다.";
        return false;
    }

    QSqlQuery cleanup(DBManager::instance().getDatabase());
    cleanup.exec("DEALLOCATE ALL");

    QString queryStr = R"(
    SELECT "RESERVATION_ID" AS 예약번호,
           TO_CHAR("RESERVATION_TIME", 'YYYY-MM-DD HH24"시"MI"분"') AS 예약시간,
           "CUSTOMER_NAME" AS 고객이름,
           FORMAT('%s-%s-%s',
                  SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                  SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                  SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
           ) AS 전화번호,
           "SERVICE_NAME" AS 시술종류,
           "DEPOSIT" AS 예약금,
           "RETOUCH" AS 리터치,
           "STATUS" AS 상태
    FROM "RESERVATION"
    JOIN "CUSTOMER" ON "RESERVATION"."CUSTOMER_ID" = "CUSTOMER"."CUSTOMER_PHONE"
    JOIN "SERVICE_TYPE" ON "RESERVATION"."SERVICE_TYPE" = "SERVICE_TYPE"."SERVICE_ID"
    WHERE "RESERVATION"."RESERVATION_TIME"::date = :reservationDate
    ORDER BY "RESERVATION_TIME";
    )";

    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(queryStr);
    query.bindValue(":reservationDate", date);

    if (!query.exec()) {
        qDebug() << "❌ 쿼리 실패:" << query.lastError().text();
        return false;
    }

    // ✅ 새 모델 생성
    ReservationModel* model = new ReservationModel(tableView); // 부모 설정: 안전하게 Qt가 소유
    model->setDataFromQuery(query);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return true;
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

bool QueryManager::fillCustomerData(QTableView* tableView,QString name, QString phone)
{
    if (!DBManager::instance().isConnected()) {
        qDebug() << "❌ DB 연결이 안 되어 있습니다.";
        return false;
    }
    QString queryStr;
    QSqlQuery query(DBManager::instance().getDatabase());
    if (name.isEmpty() && phone.isEmpty()) {
        queryStr = R"(
            SELECT 
              c."CUSTOMER_NAME" AS 고객명,
              FORMAT('%s-%s-%s',
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)) AS 전화번호,
              COUNT(r."RESERVATION_ID") AS 예약횟수
            FROM "CUSTOMER" c
            LEFT JOIN "RESERVATION" r ON r."CUSTOMER_ID" = c."CUSTOMER_PHONE"
            GROUP BY 
              c."CUSTOMER_NAME", c."CUSTOMER_PHONE"
            ORDER BY 
              c."CUSTOMER_NAME";

        )";
        query.prepare(queryStr);
    }
    else {
        queryStr = R"(
           SELECT 
              c."CUSTOMER_NAME" AS 고객명,
              FORMAT('%s-%s-%s',
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)) AS 전화번호,
              COUNT(r."RESERVATION_ID") AS 예약횟수
            FROM "CUSTOMER" c
            LEFT JOIN "RESERVATION" r ON r."CUSTOMER_ID" = c."CUSTOMER_PHONE"
            WHERE 
              (COALESCE(CAST(:phone AS TEXT), '') = '' OR c."CUSTOMER_PHONE"::TEXT ILIKE :phone)
              AND (COALESCE(:name, '') = '' OR c."CUSTOMER_NAME" ILIKE :name)
            GROUP BY 
              c."CUSTOMER_NAME", c."CUSTOMER_PHONE"
            ORDER BY 
              c."CUSTOMER_NAME";

        )";
        query.prepare(queryStr);
        query.bindValue(":name", name);
        query.bindValue(":phone", "%" + phone + "%");
    }
    
    

    if (!query.exec()) {
        qDebug() << "❌ 쿼리 실패:" << query.lastError().text();
        return false;
    }

    // ✅ 새 모델 생성
    CustomerModel* model = new CustomerModel(tableView); // 부모 설정: 안전하게 Qt가 소유
    model->setDataFromQuery(query);
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return true;
}
