#include "CustomerDetail.h"
#include "ui_CustomerDetail.h"
#include "DBManager.h"
#include <QFileDialog>
#include <QDir>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <UtilsCustomer.h>

CustomerDetail::CustomerDetail(CustomerData& initData, QWidget* parent)
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

    connect(ui->uploadImg, &QPushButton::clicked, this, &CustomerDetail::onImageBtnClicked);
}

CustomerDetail::~CustomerDetail() {
    delete ui;
}

void CustomerDetail::loadImages() {
    // DB에서 경로 조회
    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(R"(
        SELECT image_path
        FROM customer_images
        WHERE customer_id = :cid
        ORDER BY created_at
        LIMIT 10
    )");
    //query.bindValue(":cid", customerId);
    if (!query.exec()) {
        qWarning() << "이미지 로드 실패:" << query.lastError().text();
        return;
    }

    // 불러온 경로로 버튼에 아이콘 세팅
    int idx = 0;
    while (query.next() && idx < imageBtns.size()) {
        QString path = query.value("image_path").toString();
        QIcon ico(path);
        imageBtns[idx]->setIcon(ico);
        ++idx;
    }
    // 나머지는 default 아이콘
    for (; idx < imageBtns.size(); ++idx) {
        // 이미 UI에 default 세팅해뒀다면 별도 처리 불필요
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

    // 버튼 아이콘 갱신
    //imageBtns[index]->setIcon(QIcon(srcFile));
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
                , "RESERVATION_ID"
                )
            VALUES
                ( :customerId    
                , :imgPath      
                , :imgNum        
                , :reservationId 
                )
    )");
    query.bindValue(":customerId", idStr);
    query.bindValue(":imgPath", dstPath);
    query.bindValue(":imgNum", numStr);

    if (!query.exec()) {
        qWarning() << "이미지 DB 저장 실패:" << query.lastError().text();
    }
}
