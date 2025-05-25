#include "CustomerDetail.h"
#include "ui_CustomerDetail.h"
#include "DBManager.h"
#include <QFileDialog>
#include <QDir>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <UtilsCustomer.h>
#include <QuaryManager.h>

CustomerDetail::CustomerDetail(const CustomerData& initData, QWidget* parent)
    : QDialog(parent), ui(new Ui::CustomerDetail)
{
    ui->setupUi(this);
    
    setWindowTitle("고객 정보");
    data = initData;

    ui->customerName->setText(data.customerName);
    ui->customerPhone->setText(data.customerPhone);
    ui->birthDate->setDate(data.birthDate);
    data.gender ? ui->gender1->setChecked(true) : ui->gender0->setChecked(true);
    ui->visitRoot->setText(data.visitRoot);
    ui->address->setText(data.address);
    ui->memo->setPlainText(data.memo);

    // labelImg0 ~ labelImg9 를 한 번에 리스트로 관리
    imageLabels = {
        ui->labelImg0,  ui->labelImg1,  ui->labelImg2,  ui->labelImg3,   ui->labelImg4,
        ui->labelImg5,  ui->labelImg6,  ui->labelImg7,  ui->labelImg8,   ui->labelImg9
    };

    loadImages();

	reservation.searchReservationByCustomer(ui->reservationView, data.customerId);

    connect(ui->uploadImg, &QPushButton::clicked, this, &CustomerDetail::onImageBtnClicked);
}

CustomerDetail::~CustomerDetail() {
    delete ui;
}

void CustomerDetail::loadImages() {
    // DB에서 경로 조회
//    qDebug() << "labels:" << imageLabels;
    QSqlQuery query(DBManager::instance().getDatabase());
    QString sql = QStringLiteral(R"(
                        SELECT
                             "IMAGE_ID",
                             "CUSTOMER_ID",
                             "IMG_PATH",
                             "IMG_NUM"
                        FROM public."IMAGEDATA"
                        WHERE "CUSTOMER_ID" = %1
                        ORDER BY "IMG_NUM"
                    )").arg(data.customerId);

    if (!query.exec(sql)) {
        qWarning() << "이미지 경로 가져오기 실패:" << query.lastError().text();
        return;
    }
//
//
//    // ① DB에서 가져온 경로들을 QStringList에 담기
    int index = 0;
    while (query.next()) {
    //    // SELECT 문에서 IMG_PATH 컬럼(alias)를 사용했다면
        QString path = query.value("IMG_PATH").toString();
        QPixmap pix(path);
            if (pix.isNull()) {
                qWarning() << "이미지 로드 실패(파일 없음?):" << path;
                continue;
            }
            // 라벨 크기에 맞춰 비율 유지하며 부드럽게 리사이즈
            QPixmap scaled = pix.scaled(
                120, 120,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            );
            
            imageLabels[index]->setPixmap(scaled);
            index++;
    }
}

void CustomerDetail::onImageBtnClicked(int index) {
    // 파일 대화창
    QString srcFile = QFileDialog::getOpenFileName(this,
        tr("이미지 선택"), QDir::homePath(),
        tr("Images (*.png *.jpg *.jpeg)"));
    if (srcFile.isEmpty()) return;

    // 저장 및 DB 등록
    saveImage(index, srcFile);

}

void CustomerDetail::saveImage(int idx, const QString& srcFile) {
    // 1) 고객별 폴더 생성 (./images/{customerId}/)
    QDir rootDir(QCoreApplication::applicationDirPath() + "/images");
    if (!rootDir.exists()) rootDir.mkpath(".");
    
    QString idStr = QString::number(data.customerId);

    QDir custDir(rootDir.filePath(idStr));
    if (!custDir.exists()) custDir.mkpath(".");

    // 2) 파일 복사 (timestamp_원본이름)
    QDir dir(custDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList fileList = dir.entryList();

    QFileInfo info(srcFile);
    QString ext = info.suffix();  // 예: "png", "jpg"

    QString numStr = QString::number(fileList.count());
    QString fn = numStr + QStringLiteral(".") + ext;
    QString dstPath = custDir.filePath(fn);
    if (!QFile::copy(srcFile, dstPath)) {
        qWarning() << "이미지 복사 실패:" << srcFile << "->" << dstPath;
        return;
    }

    // 3) DB에 경로 저장
    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(R"(
        INSERT INTO public."IMAGEDATA"
                ( "CUSTOMER_ID"
                , "IMG_PATH"
                , "IMG_NUM"
                )
            VALUES
                ( :customerId    
                , :imgPath      
                , :imgNum        
                )
    )");
    query.bindValue(":customerId", idStr);
    query.bindValue(":imgPath", dstPath);
    query.bindValue(":imgNum", numStr);

    if (!query.exec()) {
        qWarning() << "이미지 DB 저장 실패:" << query.lastError().text();
    }
}
