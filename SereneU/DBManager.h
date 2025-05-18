#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql\QSqlDatabase>
#include <QtSql\QSqlTableModel>
#include <QDebug>

#include <QString>

struct DBConfig {
    QString host;
    qint16 port;
    QString database;
    QString user;
    QString password;
    QString pool_mode;
};

class DBManager
{
public:
    // 싱글톤 인스턴스 접근
    static DBManager& instance();

    // 데이터베이스 연결
    bool connectToDatabase(const QString& host, int port, const QString& dbName,
        const QString& user, const QString& password);

    // DB 연결 상태 확인
    bool isConnected() const;

    // DB 연결 종료
    void closeConnection();

    // 쿼리 실행 및 모델 반환
    QSqlQueryModel* getReservationsData(const QString& queryStr, QObject* parent = nullptr);

    QSqlDatabase& getDatabase();

    DBConfig loadDBConfig(const QString& filepath);

    QString getLastError();
private:
    QSqlDatabase db;
    DBManager();  // 생성자
    ~DBManager(); // 소멸자

    // 복사 및 대입 연산자 비활성화
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

    QString lastErrorMsg;
};

#endif // DBMANAGER_H
