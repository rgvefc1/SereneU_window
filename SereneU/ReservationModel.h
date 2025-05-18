#pragma once
#ifndef RESERVATIONMODEL_H
#define DBCREATOR_H
#include <QtSql\QSqlRecord>
#include <QtSql\QSqlField>
#include <QAbstractTableModel>
#include <QtSql\QSqlQuery>

class ReservationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ReservationModel(QObject* parent = nullptr);

    void setDataFromQuery(QSqlQuery& query);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<QStringList> tableData;   // ��� ������ ����
    QStringList headers;              // ��� �ؽ�Ʈ ����
};
#endif