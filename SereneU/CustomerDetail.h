#pragma once
#include <QDialog>
#include <QToolButton>
#include <QStringList>
#include <UtilsCustomer.h>

#include <QPushButton>
#include <UtilsReservation.h>

namespace Ui { class CustomerDetail; }

class CustomerDetail : public QDialog {
    Q_OBJECT
public:
    explicit CustomerDetail(const CustomerData& initData, QWidget* parent = nullptr);
    ~CustomerDetail();
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
signals:
    void customerUpdateSuccess();
private slots:
    void onImageBtnClicked(int index);
    void onImageButtonRightClicked(int index);
    void onCustomerUpdateBtnClicked(); // 업로드 버튼 클릭 시 첫 번째 이미지 버튼 클릭 이벤트로 처리
    void onCustomerDeleteBtnClicked();

private:
    void loadImages();       // DB에서 경로 불러와 버튼에 세팅
    void saveImage(int idx, const QString& srcFile);

    Ui::CustomerDetail* ui;
    CustomerData data;
    UtilsReservation reservation;
    QVector<QPushButton*> imageButtons;
	QVector<QString> imagePaths; // 이미지 경로 저장용
};
