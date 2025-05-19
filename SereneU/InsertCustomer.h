
#ifndef INSERTCUSTOMER_H
#define INSERTCUSTOMER_H
#include <QDialog>


namespace Ui { class insertCustomer; }
class InsertCustomer : public QDialog
{
    Q_OBJECT

public:
    explicit InsertCustomer(QWidget* parent = nullptr);
    ~InsertCustomer();

    void onSaveBtnClick();
signals:
    void onCustomerSelected(const QString& selectedInfo);  // 시그널 정의

private:
    Ui::insertCustomer* ui;
    void setupUI();
};

#endif // INSERTCUSTOMER_H
