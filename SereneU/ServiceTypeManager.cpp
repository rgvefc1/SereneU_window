#include "ServiceTypeManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

// 싱글톤 인스턴스
ServiceTypeManager& ServiceTypeManager::instance()
{
    static ServiceTypeManager instance;
    return instance;
}

// 생성자
ServiceTypeManager::ServiceTypeManager(QObject* parent)
    : QObject(parent)
{
}

// 서비스 유형 데이터 로드
void ServiceTypeManager::loadServiceTypes()
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT "SERVICE_ID", "SERVICE_NAME"
        FROM "SERVICE_TYPE"
    )");

    if (!query.exec()) {
        qDebug() << "❌ 서비스 유형 조회 실패: " << query.lastError().text();
        return;
    }

    // 기존 데이터 초기화
    serviceTypes.clear();

    // 결과 저장
    while (query.next()) {
        int id = query.value("SERVICE_ID").toInt();
        QString name = query.value("SERVICE_NAME").toString();
        serviceTypes.insert(id, name);
    }

    qDebug() << "✅ 서비스 유형 로드 완료! 총" << serviceTypes.size() << "개.";
}

// 특정 서비스 ID로 이름 가져오기
QString ServiceTypeManager::getServiceName(int serviceId) const
{
    return serviceTypes.value(serviceId, "알 수 없음");
}

// 전체 서비스 유형 반환
QMap<int, QString> ServiceTypeManager::getAllServiceTypes() const
{
    return serviceTypes;
}
