#pragma once
#include <QDialog>
#include <QToolButton>
#include <QStringList>
#include <UtilsCustomer.h>

#include <QLabel>
#include <UtilsReservation.h>

namespace Ui { class CustomerDetail; }

class CustomerDetail : public QDialog {
    Q_OBJECT
public:
    explicit CustomerDetail(const CustomerData& initData, QWidget* parent = nullptr);
    ~CustomerDetail();

private slots:
    void onImageBtnClicked(int index);

private:
    void loadImages();       // DB에서 경로 불러와 버튼에 세팅
    void saveImage(int idx, const QString& srcFile);

    Ui::CustomerDetail* ui;
    CustomerData data;
    UtilsReservation reservation;
    QVector<QLabel*> imageLabels;
};
