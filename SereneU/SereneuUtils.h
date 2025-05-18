#ifndef SERENEUUTILS_H
#define SERENEUUTILS_H

#include <QDateEdit>
#include <QTimeEdit>
#include <QString>
#include <QDateTime>
#include <QDebug>

class SereneuUtils
{
public:
    static QString createTimestamp(const QDateEdit* dateEdit, const QTimeEdit* timeEdit);  // const 포인터 사용
};

#endif // SERENEUUTILS_H
