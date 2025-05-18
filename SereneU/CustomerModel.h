#ifndef CUSTOMERMODEL_H
#define CUSTOMERMODEL_H

#include <QtSql\QSqlRecord>
#include <QtSql\QSqlField>
#include <QAbstractTableModel>
#include <QtSql\QSqlQuery>

class CustomerModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CustomerModel(QObject* parent = nullptr);
  
    void setDataFromQuery(QSqlQuery& query);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QVector<QStringList> tableData;   // 행렬 데이터 저장
    QStringList headers;              // 헤더 텍스트 저장
};

#endif // CUSTOMERMODEL_H
