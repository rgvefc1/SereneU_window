#include "UtilsCustomer.h"
#include "DBManager.h"
#include <QtSql/qsqlquery.h>
#include<QMessageBox>
#include<QtSql\qsqlerror.h>
#include <CustomerModel.h>
#include <qtableview.h>
#include <QuaryManager.h>

UtilsCustomer::UtilsCustomer()
	:db(DBManager::instance().getDatabase()),  // ① DB 연결 가져오기
	query(db)                                 // ② 이 DB로 쿼리 객체 생성
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

UtilsCustomer::~UtilsCustomer()
{
}

bool UtilsCustomer::isPhoneNumberExists(const QString& phoneNumber)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT COUNT(*) FROM "CUSTOMER"
        WHERE "CUSTOMER_PHONE" = :phone
    )");
    query.bindValue(":phone", phoneNumber);

    if (!query.exec()) {
        return true;  // 실패 시 중복된 것으로 처리
    }

    query.next();
    int count = query.value(0).toInt();
    return count > 0 ? true:false;
}

bool UtilsCustomer::searchCustomerInfo(QTableView* tableView,QString name, QString phone)
{
    QSqlQuery query;
    query.prepare(R"(
                        SELECT
                            "CUSTOMER_NAME" AS "고객이름",
                            FORMAT(
                                '%s-%s-%s',
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8 FOR 4)
                            ) AS "전화번호"
                        FROM "CUSTOMER"
                        WHERE
                            (COALESCE(CAST(:phone AS TEXT), '') = '' OR "CUSTOMER_PHONE"::TEXT ILIKE :phone)
                          AND
                            (COALESCE(:name, '') = '' OR "CUSTOMER_NAME" ILIKE :name)
                    )");
    query.bindValue(":name", name);
    query.bindValue(":phone", "%" + phone + "%");


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

bool UtilsCustomer::allCustomerInfo(QTableView* tableView)
{
    QSqlQuery query;
    query.prepare(R"(
                        SELECT
                            "CUSTOMER_NAME" AS "고객이름",
                            FORMAT(
                                '%s-%s-%s',
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 1 FOR 3),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 4 FOR 4),
                                SUBSTRING(LPAD(CAST("CUSTOMER_PHONE" AS TEXT), 11, '0') FROM 8 FOR 4)
                            ) AS "전화번호"
                        FROM "CUSTOMER"
                    )");

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

QString UtilsCustomer::insertCustomer(CustomerData data)
{
    QSqlQuery query;
    query.prepare(QueryManager::INSERT_CUSTOMER);
    query.bindValue(":name", data.customerName);
    query.bindValue(":phone", data.customerPhone);
    query.bindValue(":birthDate", data.birthDate);
    query.bindValue(":gender", data.gender);
    query.bindValue(":address", data.address);
    query.bindValue(":notes", data.memo);

    if (!query.exec()) {
        return  query.lastError().text();
    }else{
        return "success";
    }
}

bool UtilsCustomer::updateCustomer(CustomerData data)
{
    return false;
}

bool UtilsCustomer::deleteCustomer(int customerId)
{
    return false;
}

int UtilsCustomer::allCustomerCount()
{
    query.exec("SELECT COUNT(*) FROM customers");

    if (query.next()) {
        return query.value(0).toInt();
    }

    qDebug() << "❌ 고객 수 조회 실패: " << query.lastError().text();
    return 0;
}
