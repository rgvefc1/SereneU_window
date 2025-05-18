
#ifndef ADDCUSTOMER_H
#define ADDCUSTOMER_H
#include <QDialog>
#include <QStringListModel>

namespace Ui {
    class AddCustomer;
}

class AddCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit AddCustomer(int mode,QWidget* parent = nullptr);
    ~AddCustomer();

    bool isPhoneNumberExists(const QString& phoneNumber);

    void addCustomer(const QString& name, const QString& phoneNumber);

    QMap<QString, QString> sellectCustomer(QString name, QString phone);

    void onSaveClicked();
    void onSearchClicked();
signals:
    void onCustomerSelected(const QString& selectedInfo);  // 시그널 정의

private:
    Ui::AddCustomer* ui;
    QStringListModel* listModel;  // 리스트뷰에 사용할 모델

    int mode;  // 0 = 등록, 1 = 생성

    void setupUI();
};

#endif // ADDCUSTOMER_H
