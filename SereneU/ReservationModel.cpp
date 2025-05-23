﻿#include "ReservationModel.h"
#include <QtSql\QSqlRecord>
#include <QtSql\QSqlField>
#include <QtSql\QSqlQuery>

ReservationModel::ReservationModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void ReservationModel::setDataFromQuery(QSqlQuery& query)
{
    beginResetModel();

    tableData.clear();
    headers.clear();

    int colCount = query.record().count();
    for (int i = 0; i < colCount; ++i) {
        headers << query.record().fieldName(i);
    }

    while (query.next()) {
        QStringList row;
        for (int i = 0; i < colCount; ++i) {
            if (headers.at(i) == "리터치") {
                query.value(i).toBool() ? row << "✔️" : row << "❌";
            }
            else if (headers.at(i) == "상태") {
                switch (query.value(i).toInt()) {
                case 0: row << "예약"; break;
                case 1: row << "리터치"; break;
                case 2: row << "완료"; break;
                case 3: row << "취소"; break;
                case 4: row << "노쇼"; break;
                default: row << "예약"; break;
                }
            }
            else {
                row << query.value(i).toString();
                
            }
        }
        tableData.append(row);
    }

    endResetModel();
}

int ReservationModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return tableData.size();
}

int ReservationModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return headers.size();
}

QVariant ReservationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    if (role == Qt::DisplayRole) {
        int row = index.row();
        int col = index.column();

        if (row >= 0 && row < tableData.size() && col >= 0 && col < headers.size()) {
            return tableData[row][col];
        }

        return QVariant();
    }
    if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;

    return QVariant();
    
}

QVariant ReservationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && section < headers.size())
        return headers[section];

    return QVariant();
}
