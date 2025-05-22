#pragma once

#include <QString>

// QueryManager: SQL 쿼리를 중앙에서 관리하는 헤더 파일
// 사용 예: QSqlQuery query(db);
//       query.prepare(QueryManager::RESERVATION_BY_DATE);
//       query.bindValue(...);
//
namespace QueryManager {
    // 예약 일자별 조회
    inline const QString RESERVATION_BY_DATE = R"(
             SELECT
                 r."RESERVATION_ID"         AS "예약번호",
                 TO_CHAR(
                     r."RESERVATION_TIME",
                     'YYYY-MM-DD HH24"시"MI"분"'
                 )                           AS "예약시간",
                 r."CUSTOMER_ID"            AS "예약자ID",
                 c."CUSTOMER_NAME"          AS "고객이름",
                 FORMAT(
                     '%s-%s-%s',
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                     SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
                 )                           AS "전화번호",
                 r."SERVICE_ID"             AS "시술ID",
                 s."SERVICE_NAME"           AS "시술종류",
                 r."PRICE"                  AS "시술가격",
                 r."DEPOSIT"                AS "예약금",
                 r."RETOUCH"                AS "리터치",
                 r."STATUS"                 AS "상태",
                 r."NOTES"                  AS "메모"
             FROM "RESERVATION" r
             JOIN "CUSTOMER"     c ON r."CUSTOMER_ID" = c."CUSTOMER_ID"
             JOIN "SERVICE_TYPE" s ON r."SERVICE_ID"   = s."SERVICE_ID"
        WHERE r."RESERVATION_TIME"::date = :reservationDate
        ORDER BY r."RESERVATION_TIME"
    )";

    // 고객별 예약 조회
    inline const QString RESERVATION_BY_CUSTOMER = R"(
        SELECT
            r."RESERVATION_ID"     AS "예약번호",
            TO_CHAR(r."RESERVATION_TIME", 'YYYY-MM-DD HH24"시"MI"분"') AS "예약시간",
            r."CUSTOMER_ID"        AS "예약자ID",
            c."CUSTOMER_NAME"      AS "고객이름",
            FORMAT(
                '%s-%s-%s',
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
            )                       AS "전화번호",
            r."SERVICE_ID"         AS "시술ID",
            s."SERVICE_NAME"       AS "시술종류",
            s."PRICE"              AS "시술가격",
            r."DEPOSIT"            AS "예약금",
            r."RETOUCH"            AS "리터치",
            r."STATUS"             AS "상태",
            r."NOTES"              AS "메모",
            r."CANCELLATION_REASON" AS "취소사유"
        FROM "RESERVATION" r
        JOIN "CUSTOMER"     c ON r."CUSTOMER_ID" = c."CUSTOMER_PHONE"
        JOIN "SERVICE_TYPE" s ON r."SERVICE_ID"   = s."SERVICE_ID"
        WHERE r."CUSTOMER_ID" = :customerId
        ORDER BY r."RESERVATION_TIME"
    )";

    // 전체 예약 조회
    inline const QString RESERVATION_ALL = R"(
        SELECT
            r."RESERVATION_ID"         AS "예약번호",
            TO_CHAR(
                r."RESERVATION_TIME",
                'YYYY-MM-DD HH24"시"MI"분"'
            )                           AS "예약시간",
            r."CUSTOMER_ID"            AS "예약자ID",
            c."CUSTOMER_NAME"          AS "고객이름",
            FORMAT(
                '%s-%s-%s',
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                SUBSTRING(LPAD(CAST(c."CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8)
            )                           AS "전화번호",
            r."SERVICE_ID"             AS "시술ID",
            s."SERVICE_NAME"           AS "시술종류",
            s."PRICE"                  AS "시술가격",
            r."DEPOSIT"                AS "예약금",
            r."RETOUCH"                AS "리터치",
            r."STATUS"                 AS "상태",
            r."NOTES"                  AS "메모",
            r."CANCELLATION_REASON"    AS "취소사유"
        FROM "RESERVATION" r
        JOIN "CUSTOMER"     c ON r."CUSTOMER_ID" = c."CUSTOMER_PHONE"
        JOIN "SERVICE_TYPE" s ON r."SERVICE_ID"   = s."SERVICE_ID"
        ORDER BY r."RESERVATION_TIME"
    )";

    inline const QString INSERT_CUSTOMER = R"(
        INSERT INTO "CUSTOMER" ("CUSTOMER_NAME", "CUSTOMER_PHONE","BIRTH_DATE","GENDER","ADDRESS","NOTES","VISIT_ROOT")
        VALUES (:name, :phone,:birthDate,:gender,:address,:notes,:visitroot)
    )";

    inline const QString SEARCH_CUSTOMER_INFO = R"(
                        SELECT
                            "CUSTOMER_ID" AS "고객번호",
                            "CUSTOMER_NAME" AS "고객이름",
                            FORMAT(
                                '%s-%s-%s',
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8 FOR 4)
                            ) AS "전화번호",
                            "BIRTH_DATE" AS "생년월일",
                            "GENDER" AS "성별",
                            "ADDRESS" AS "주소", "VISIT_ROOT" AS "방문경로","NOTES" AS "메모"
                        FROM "CUSTOMER"
                        WHERE
                            (COALESCE(CAST(:phone AS TEXT), '') = '' OR "CUSTOMER_PHONE"::TEXT ILIKE :phone)
                          AND
                            (COALESCE(:name, '') = '' OR "CUSTOMER_NAME" ILIKE :name)
    )";
    inline const QString ALL_CUSTOMER_INFO = R"(
                        SELECT
                            "CUSTOMER_ID" AS "고객번호",
                            "CUSTOMER_NAME" AS "고객이름",
                            FORMAT(
                                '%s-%s-%s',
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8 FOR 4)
                            ) AS "전화번호",
                            "BIRTH_DATE" AS "생년월일",
                            "GENDER" AS "성별",
                            "ADDRESS" AS "주소", 
                            "VISIT_ROOT" AS "방문경로",
                            "NOTES" AS "메모"
                        FROM "CUSTOMER"
    )";
    inline const QString PHONE_NUMBER_EXISTS = R"(
                    SELECT COUNT(*) FROM "CUSTOMER"
                            WHERE "CUSTOMER_PHONE" = :phone
    )";
    inline const QString INSERT_RESERVATION = R"(
        INSERT INTO public."RESERVATION" ("CUSTOMER_ID","CUSTOMER_NAME","CUSOTMER_PHONE","SERVICE_ID","SERVICE_NAME","PRICE","DEPOSIT","RESERVATION_TIME","STATUS","RETOUCH","NOTES") 
                                  VALUES (:customer_id,:customer_name,:customer_phone,:service_id,:service_name,:price,:deposit,:reservation_time,:status,:retouch,:notes)
    )";
    inline const QString UPDATE_RESERVATION = R"(
        UPDATE public."RESERVATION"
            SET
                "CUSTOMER_ID"       = :customer_id,
                "CUSTOMER_NAME"     = :customer_name,
                "CUSOTMER_PHONE"    = :customer_phone,
                "SERVICE_ID"        = :service_id,
                "SERVICE_NAME"      = :service_name,
                "PRICE"             = :price,
                "DEPOSIT"           = :deposit,
                "RESERVATION_TIME"  = :reservation_time,
                "STATUS"            = :status,
                "RETOUCH"           = :retouch,
                "NOTES"             = :notes
            WHERE
                "RESERVATION_ID"    = :reservation_id;
    )";
} // namespace QueryManager
