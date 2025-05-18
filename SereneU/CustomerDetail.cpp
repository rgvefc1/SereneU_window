#include "CustomerDetail.h"
#include "ui_CustomerDetail.h"
#include "DBManager.h"
#include <QFileDialog>
#include <QDir>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

CustomerDetail::CustomerDetail(const QString& customerId, QWidget* parent)
    : QDialog(parent), ui(new Ui::CustomerDetail), customerId(customerId)
{
    ui->setupUi(this);

    // ① 버튼 리스트에 넣기
    /*imageBtns = {
        ui->imageBtn1, ui->imageBtn2, ui->imageBtn3,
        ui->imageBtn4, ui->imageBtn5, ui->imageBtn6,
        ui->imageBtn7, ui->imageBtn8, ui->imageBtn9,
        ui->imageBtn10
    };*/
    //// ① 먼저 모두 default 아이콘으로 세팅
    //QIcon defaultIco(":/images/defaultImg.png");
    //QSize iconSz = QSize(100, 100);  // Designer에서 설정한 크기와 동일하게
    //for (auto btn : imageBtns) {
    //    btn->setIcon(defaultIco);
    //    btn->setIconSize(iconSz);
    //    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    //}
    //// ② 클릭 시그널 연결 (람다 + [=] 캡처)
    //for (int i = 0; i < imageBtns.size(); ++i) {
    //    connect(imageBtns[i], &QToolButton::clicked, [this, i]() {
    //        onImageBtnClicked(i);
    //        });
    //}

    //// ③ 이미지 로딩
    //loadImages();
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
    query.bindValue(":cid", customerId);
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
    imageBtns[index]->setIcon(QIcon(srcFile));
}

void CustomerDetail::saveImage(int idx, const QString& srcFile) {
    // 1) 고객별 폴더 생성 (./images/{customerId}/)
    QDir rootDir(QCoreApplication::applicationDirPath() + "/images");
    if (!rootDir.exists()) rootDir.mkpath(".");
    QDir custDir(rootDir.filePath(customerId));
    if (!custDir.exists()) custDir.mkpath(".");

    // 2) 파일 복사 (timestamp_원본이름)
    QString fn = QString::number(QDateTime::currentSecsSinceEpoch())
        + "_" + QFileInfo(srcFile).fileName();
    QString dstPath = custDir.filePath(fn);
    if (!QFile::copy(srcFile, dstPath)) {
        qWarning() << "이미지 복사 실패:" << srcFile << "->" << dstPath;
        return;
    }

    // 3) DB에 경로 저장
    QSqlQuery query(DBManager::instance().getDatabase());
    query.prepare(R"(
        INSERT INTO customer_images (customer_id, image_path)
        VALUES (:cid, :path)
    )");
    query.bindValue(":cid", customerId);
    query.bindValue(":path", dstPath);
    if (!query.exec()) {
        qWarning() << "이미지 DB 저장 실패:" << query.lastError().text();
    }
}
