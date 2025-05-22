#include "DBManager.h"
#include <QtSql\QSqlError>
#include <QDebug>
#include <qevent.h>
#include <QFile>
#include <QXmlStreamReader>
#include <QtSql/qsqlquery.h>

// 싱글톤 인스턴스
DBManager& DBManager::instance()
{
    static DBManager instance;
    return instance;
}

// 생성자
DBManager::DBManager()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    DBConfig config = loadDBConfig("../config/dbConfig.xml");
    db.setHostName(config.host);
    db.setPort(config.port);
    db.setDatabaseName(config.database);
    db.setUserName(config.user);
    db.setPassword(config.password);
    db.setConnectOptions("connect_timeout=3");


}

// 소멸자
DBManager::~DBManager()
{
    closeConnection();
}

// 데이터베이스 연결
bool DBManager::connectToDatabase(const QString& host, int port, const QString& dbName,
    const QString& user, const QString& password)
{
    if (db.isOpen()) {
        qDebug() << "이미 연결된 DB가 있습니다.";
        return true;
    }

    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions("connect_timeout=3");

    if (!db.open()) {
        lastErrorMsg = db.lastError().text();
        qDebug() << "❌ DB 연결 실패: " << db.lastError().text();
        return false;
    }

    qDebug() << "✅ DB 연결 성공!";

    // ── 여기에 세션 타임존 고정 ──
    {
        QSqlQuery tzQuery(db);
        // UTC로 고정: LocalTime↔UTC 변환 없이 그대로 주고받기
        if (!tzQuery.exec("SET TIME ZONE 'Asia/Seoul'")) {
            qWarning() << "타임존 고정 실패:" << tzQuery.lastError().text();
            // 실패해도 계속 진행할 수 있습니다.
        }
        else {
            qDebug() << "세션 타임존을 UTC로 설정했습니다.";
        }
    }
    return true;
}

// 연결 상태 확인
bool DBManager::isConnected() const
{
    return db.isOpen();
}

// DB 연결 종료
void DBManager::closeConnection()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "🔒 DB 연결이 종료되었습니다.";
    }
}

QSqlQueryModel* DBManager::getReservationsData(const QString& queryStr, QObject* parent)
{
    if (!db.isOpen()) {
        qDebug() << "❌ DB가 열려있지 않습니다.";
        return nullptr;
    }

    QSqlQueryModel* model = new QSqlQueryModel(parent);
    model->setQuery(queryStr, db);

    if (model->lastError().isValid()) {
        qDebug() << "❌ 쿼리 실행 실패: " << model->lastError().text();
        delete model;
        return nullptr;
    }

    qDebug() << "✅ 쿼리 실행 성공! 가져온 행 수: " << model->rowCount();
    return model;
}
QSqlDatabase& DBManager::getDatabase()
{
    return db;
}

DBConfig DBManager::loadDBConfig(const QString& filepath)
{
    DBConfig config;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "XML 파일 열기 실패: " << filepath;
        return config;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "host") config.host = xml.readElementText();
            else if (xml.name() == "port") config.port = xml.readElementText().toInt();
            else if (xml.name() == "database") config.database = xml.readElementText();
            else if (xml.name() == "user") config.user = xml.readElementText();
            else if (xml.name() == "password") config.password = xml.readElementText();
            else if (xml.name() == "pool_mode") config.pool_mode = xml.readElementText();
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML parsing error: " << xml.errorString();
    }

    file.close();
    return config;
}

QString DBManager::getLastError() 
{
    return lastErrorMsg;
}
