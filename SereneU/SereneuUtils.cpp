#include "SereneuUtils.h"

QString SereneuUtils::createTimestamp(const QDateEdit* dateEdit, const QTimeEdit* timeEdit)
{
    // 포인터 유효성 검사
    if (!dateEdit || !timeEdit) {
        qDebug() << "❌ 유효하지 않은 DateEdit 또는 TimeEdit 포인터입니다.";
        return QString();
    }

    // 날짜와 시간 가져오기
    QDate selectedDate = dateEdit->date();
    QTime selectedTime = timeEdit->time();

    // 유효성 검사
    if (!selectedDate.isValid() || !selectedTime.isValid()) {
        qDebug() << "❌ 유효하지 않은 날짜 또는 시간입니다.";
        return QString();
    }

    // QDateTime으로 결합
    QDateTime dateTime(selectedDate, selectedTime);

    // 타임스탬프 형식으로 문자열 반환
    QString timestamp = dateTime.toString("yyyy-MM-dd HH:mm:ss");

    qDebug() << "✅ 생성된 Timestamp: " << timestamp;
    return timestamp;
}
