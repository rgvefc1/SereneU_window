#include "CustomerModel.h"
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
            row << query.value(i).toString();
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
        if (section == 0)
            return "고객명";
        else if (section == 1)
            return "전화번호";
        else if (section == 2)
            return "예약횟수";
    }

    return QVariant();
}

