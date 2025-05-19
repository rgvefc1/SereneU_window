#include <qstring.h>
#include <QWidget>                // QWidget
#include <QMap>                   // QMap
#include <QtSql/QSqlDatabase>     // QSqlDatabase
#include <QtSql/QSqlQuery>        // QSqlQuery
#include <qtableview.h>
#include <qdatetime.h>

#ifndef UTILSCUSTOMER_H
#define UTILSCUSTOMER_H

struct CustomerData {
    int       customerId;        // 고객 ID
    QString   customerName;      // 고객 이름
    QDate     birthDate;         // 생년월일(YYYY MM DD)
    bool   gender;               // 0 여자 / 1 남자
    QDateTime createdAt;         // 가입날자
    QString   customerPhone;     // 전화번호
    QString   address;           // 주소
    int       visitCount;        // 방문 횟수
    QDateTime lastVisitDate;     // 마지막 방문 날자

    CustomerData() = default;
};

class UtilsCustomer
{

public:
    explicit UtilsCustomer();
    ~UtilsCustomer();

    bool isPhoneNumberExists(const QString& phoneNumber);

    bool searchCustomerInfo(QTableView* parent,QString name, QString phone);
    bool allCustomerInfo(QTableView* tableView = nullptr);
    bool insertCustomer(QWidget* parent, const QString& name, const QString& phoneNumber);
    bool updateCustomer(const QString& name, const QString& phoneNumber);
    bool deleteCustomer(const QString& name, const QString& phoneNumber);

    int allCustomerCount();
private:
    QSqlDatabase db;
    QSqlQuery query;
    QChar mode;
};
#endif UTILSCUSTOMER_H
