﻿#include "CustomerModel.h"
#include <QtSql\QSqlRecord>
#include <QtSql\QSqlField>
#include <QtSql\QSqlQuery>

CustomerModel::CustomerModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void CustomerModel::setDataFromQuery(QSqlQuery& query)
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
            if (headers.at(i) == "성별") {
                query.value(i).toBool() ? row << "남" : row << "여";
            }
            else {
                row << query.value(i).toString();
            }
        }
        tableData.append(row);
    }

    endResetModel();
}


int CustomerModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return tableData.size();
}

int CustomerModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return headers.size();  // 이름, 전화번호
}

QVariant CustomerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // ✅ 중앙 정렬은 항상 반환
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

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

QVariant CustomerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < headers.size())
            return headers.at(section);          // ③ 동적 반환
        return {};                               // 범위 초과 시 빈 QVariant
    }

    return section + 1;
}

