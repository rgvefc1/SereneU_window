#ifndef SERVICETYPEMANAGER_H
#define SERVICETYPEMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

class ServiceTypeManager : public QObject
{
    Q_OBJECT

public:
    static ServiceTypeManager& instance();

    void loadServiceTypes();
    QString getServiceName(int serviceId) const;
    QMap<int, QString> getAllServiceTypes() const;
    
private:
    explicit ServiceTypeManager(QObject* parent = nullptr);
    ~ServiceTypeManager() = default;
    QMap<int, QString> serviceTypes;  // 서비스 ID와 이름 저장
    Q_DISABLE_COPY(ServiceTypeManager)  // 복사 방지
};

#endif // SERVICETYPEMANAGER_H
