#include "UtilsReservation.h"
#include "DBManager.h"
#include <QtSql/qsqlquery.h>
#include<QMessageBox>
#include<QtSql\qsqlerror.h>
#include <CustomerModel.h>
#include <qtableview.h>
#include <ReservationModel.h>
#include <QuaryManager.h>

UtilsReservation::UtilsReservation()
	:db(DBManager::instance().getDatabase()),  // ① DB 연결 가져오기
	query(db)
{
	// ③ DB가 열려 있지 않다면 열어주기 (에러 시 메시지 박스)
	if (!db.isOpen()) {
		if (!db.open()) {
			QMessageBox::critical(nullptr,
				QObject::tr("데이터베이스 오류"),
				db.lastError().text());
		}
	}
}

UtilsReservation::~UtilsReservation() = default;

bool UtilsReservation::searchReservationByDate(QTableView* tableview, const QString& date)
{
    if (!DBManager::instance().isConnected()) {
        qDebug() << "❌ DB 연결이 안 되어 있습니다.";
        return false;
    }

    QSqlQuery cleanup(DBManager::instance().getDatabase());
    cleanup.exec("DEALLOCATE ALL");

    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(QueryManager::RESERVATION_BY_DATE);
    query.bindValue(":reservationDate", date);

    if (!query.exec()) {
        qDebug() << "❌ 쿼리 실패:" << query.lastError().text();
        return false;
    }

    // ✅ 새 모델 생성
    ReservationModel* model = new ReservationModel(tableview); // 부모 설정: 안전하게 Qt가 소유
    model->setDataFromQuery(query);
    tableview->setModel(model);
    tableview->resizeColumnsToContents();
    tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return true;
}

bool UtilsReservation::searchReservationByCustomer(QTableView* tableview, QString& customerId)
{
    if (!DBManager::instance().isConnected()) {
        qDebug() << "❌ DB 연결이 안 되어 있습니다.";
        return false;
    }

    QSqlQuery cleanup(DBManager::instance().getDatabase());
    cleanup.exec("DEALLOCATE ALL");

    QString queryStr = R"(
                            SELECT
                                "RESERVATION_ID"     AS 예약번호,
                                TO_CHAR(
                                    "RESERVATION_TIME", 'YYYY-MM-DD HH24"시"MI"분"'
                                )                     AS 예약시간,
                                "CUSTOMER_NAME"      AS 고객이름,
                                FORMAT(
                                    '%s-%s-%s',
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
                                )                     AS 전화번호,
                                "SERVICE_NAME"       AS 시술종류,
                                "DEPOSIT"            AS 예약금,
                                "RETOUCH"            AS 리터치,
                                "STATUS"             AS 상태
                            FROM "RESERVATION"
                            JOIN "CUSTOMER"
                              ON "RESERVATION"."CUSTOMER_ID" = "CUSTOMER"."CUSTOMER_PHONE"
                            JOIN "SERVICE_TYPE"
                              ON "RESERVATION"."SERVICE_TYPE" = "SERVICE_TYPE"."SERVICE_ID"
                            WHERE "RESERVATION"."CUSTOMER_ID" = :customerId
                            ORDER BY "RESERVATION_TIME"
                        )";

    query.prepare(queryStr);
    query.bindValue(":customerId", customerId);

    if (!query.exec()) {
        qDebug() << "❌ 쿼리 실패:" << query.lastError().text();
        return false;
    }

    // ✅ 새 모델 생성
    ReservationModel* model = new ReservationModel(tableview); // 부모 설정: 안전하게 Qt가 소유
    model->setDataFromQuery(query);
    tableview->setModel(model);
    tableview->resizeColumnsToContents();
    tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return true;
}

bool UtilsReservation::searchReservationAll(QTableView* tableview)
{
    if (!DBManager::instance().isConnected()) {
        qDebug() << "❌ DB 연결이 안 되어 있습니다.";
        return false;
    }

    QSqlQuery cleanup(DBManager::instance().getDatabase());
    cleanup.exec("DEALLOCATE ALL");

    QString queryStr = R"(
                            SELECT
                                "RESERVATION_ID"     AS 예약번호,
                                TO_CHAR(
                                    "RESERVATION_TIME", 'YYYY-MM-DD HH24"시"MI"분"'
                                )                     AS 예약시간,
                                "CUSTOMER_NAME"      AS 고객이름,
                                FORMAT(
                                    '%s-%s-%s',
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                    SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
                                )                     AS 전화번호,
                                "SERVICE_NAME"       AS 시술종류,
                                "DEPOSIT"            AS 예약금,
                                "RETOUCH"            AS 리터치,
                                "STATUS"             AS 상태
                            FROM "RESERVATION"
                            JOIN "CUSTOMER"
                              ON "RESERVATION"."CUSTOMER_ID" = "CUSTOMER"."CUSTOMER_PHONE"
                            JOIN "SERVICE_TYPE"
                              ON "RESERVATION"."SERVICE_TYPE" = "SERVICE_TYPE"."SERVICE_ID"
                            ORDER BY "RESERVATION_TIME"
                        )";

    query.prepare(queryStr);

    if (!query.exec()) {
        qDebug() << "❌ 쿼리 실패:" << query.lastError().text();
        return false;
    }

    // ✅ 새 모델 생성
    ReservationModel* model = new ReservationModel(tableview); // 부모 설정: 안전하게 Qt가 소유
    model->setDataFromQuery(query);
    tableview->setModel(model);
    tableview->resizeColumnsToContents();
    tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return true;
}

bool UtilsReservation::insertReservation(ReservationData data)
{
    query.prepare(QueryManager::INSERT_RESERVATION);

    // 파라미터 바인딩
    query.bindValue(":customer_id", data.customerId);
    query.bindValue(":customer_name", data.customerName);
    query.bindValue(":customer_phone", data.customerPhone);
    query.bindValue(":service_id", data.serviceId);
    query.bindValue(":service_name", data.serviceName);
    query.bindValue(":price", data.price);
    query.bindValue(":deposit", data.deposit);
    query.bindValue(":reservation_time", data.reservationTime);
    query.bindValue("status", data.status);
    query.bindValue(":retouch", data.retouch);
    query.bindValue(":notes", data.notes);

    // 쿼리 실행
    if (query.exec()) {
        qDebug() << "예약 성공";
        return true;
    }
    else {
        qDebug() << "❌ 예약등록 쿼리 실패:" << query.lastError().text();
        return false;
    }
}

bool UtilsReservation::updateReservation(ReservationData data)
{
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
    query.bindValue(":reservation_time", data.reservationTime);
    query.bindValue(":customer_id", data.customerId);
    query.bindValue(":service_type", data.serviceName);
    query.bindValue(":retouch", data.retouch);
    query.bindValue(":deposit", data.deposit);
    query.bindValue(":reservation_id", data.reservationId);

    // 쿼리 실행
    if (!query.exec()) {
        qWarning() << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        qWarning() << "[updateReservation] 조건에 맞는 행이 없습니다!"
            << "reservationId =" << data.reservationId;
        return false;
    }
    DBManager::instance().getDatabase().commit();
    return true;
}

bool UtilsReservation::deleteReservation(int reservationId)
{
    // DELETE 쿼리 준비
    query.prepare(R"(
        DELETE FROM "RESERVATION"
        WHERE "RESERVATION_ID" = :reservation_id
    )");
    query.bindValue(":reservation_id", reservationId);

    // 쿼리 실행
    if (!query.exec()) {
        qWarning() << "[deleteReservation] 쿼리 실행 실패:" << query.lastError().text();
        return false;
    }

    // 영향을 받은 행(row) 수 체크
    if (query.numRowsAffected() == 0) {
        qWarning() << "[deleteReservation] 조건에 맞는 예약이 없습니다! reservationId =" << reservationId;
        return false;
    }

    // 변화 확정 (트랜잭션 커밋)
    if (!DBManager::instance().getDatabase().commit()) {
        qWarning() << "[deleteReservation] 커밋 실패:" << DBManager::instance().getDatabase().lastError().text();
        return false;
    }

    return true;
}
