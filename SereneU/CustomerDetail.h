#pragma once
#include <QDialog>
#include <QToolButton>
#include <QStringList>

namespace Ui { class CustomerDetail; }

class CustomerDetail : public QDialog {
    Q_OBJECT
public:
    explicit CustomerDetail(const QString& customerId, QWidget* parent = nullptr);
    ~CustomerDetail();

private slots:
    void onImageBtnClicked(int index);

private:
    void loadImages();       // DB에서 경로 불러와 버튼에 세팅
    void saveImage(int idx, const QString& srcFile);

    Ui::CustomerDetail* ui;
    QString customerId;
    QList<QToolButton*> imageBtns;
};
