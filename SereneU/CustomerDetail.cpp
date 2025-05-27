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
#include <QDesktopServices>
#include <QMessageBox>
#include <QMouseEvent>
#include <QUrl>
#include <QtCharts/QChartView>

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
    imageButtons = {
        ui->imgBtn0,  ui->imgBtn1,  ui->imgBtn2,  ui->imgBtn3,   ui->imgBtn4,
        ui->imgBtn5,  ui->imgBtn6,  ui->imgBtn7,  ui->imgBtn8,   ui->imgBtn9
    };

	for (int i = 0; i < imageButtons.size(); ++i) {
        imageButtons[i]->installEventFilter(this);
	}

    loadImages();

	reservation.searchReservationByCustomer(ui->reservationView, data.customerId);

    connect(ui->uploadImg, &QPushButton::clicked, this, &CustomerDetail::onImageBtnClicked);
    connect(ui->updateCustomer, &QPushButton::clicked, this, &CustomerDetail::onCustomerUpdateBtnClicked);
    connect(ui->deleteCustomer, &QPushButton::clicked, this, &CustomerDetail::onCustomerDeleteBtnClicked);
}

CustomerDetail::~CustomerDetail() {
    delete ui;
}

bool CustomerDetail::eventFilter(QObject* watched, QEvent* event) {
    // 마우스 버튼 클릭 이벤트만 처리
    if (event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);

        // imageButtons 와 imagePaths 가 같은 순서로 매핑되어 있어야 합니다
        for (int i = 0; i < imageButtons.size(); ++i) {
            if (watched == imageButtons[i]) {
                if (me->button() == Qt::LeftButton) {
                    // 왼쪽 클릭 → 기본 사진뷰어 열기
                    QDesktopServices::openUrl(
                        QUrl::fromLocalFile(imagePaths[i])
                    );
                }
                else if (me->button() == Qt::RightButton) {
                    // 오른쪽 클릭 → 커스텀 슬롯 호출
                    onImageButtonRightClicked(i);
                }
                return true;  // 이 이벤트는 처리했음을 알림
            }
        }
    }
    // 나머지는 기본 처리
    return QDialog::eventFilter(watched, event);
}

void CustomerDetail::onImageButtonRightClicked(int index)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
        tr("삭제 확인"),      // 창 제목
        tr("해당 사진을 삭제 하시겠습니까?"), // 본문 메시지
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 유효 범위 체크
        if (index < 0 || index >= imageButtons.size()) {
            qWarning() << "잘못된 인덱스:" << index;
            return;
        }

        // 1) DB에서 해당 IMAGE_ID 삭제
        QSqlQuery query(DBManager::instance().getDatabase());
        QString sql = QueryManager::DELETE_IMAGEDATA.arg(imagePaths[index]);
        if (!query.exec(sql)) {
            qWarning() << "DB이미지 삭제 실패:" << query.lastError().text();
            return;
        }

        // 2) 파일 시스템에서 이미지 파일 삭제
        const QString& fileToRemove = imagePaths[index];
        if (!QFile::remove(fileToRemove)) {
            qWarning() << "파일 삭제 실패:" << fileToRemove;
        }
        imagePaths.clear();
        loadImages();
    }
    else {
        return;
    }
}

void CustomerDetail::onCustomerUpdateBtnClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
        tr("수정 확인"),      // 창 제목
        tr("수정하시겠습니까?"), // 본문 메시지
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        data.customerName = ui->customerName->text();
        data.customerPhone = ui->customerPhone->text().replace("-", "");
        data.birthDate = ui->birthDate->date();
        if (ui->gender0->isChecked()) {
            data.gender = false;
        }
        else {
            data.gender = true;
        }
        data.visitRoot = ui->visitRoot->text();
        data.address = ui->address->text();
        data.memo = ui->memo->toPlainText();
        QSqlQuery query(DBManager::instance().getDatabase());
        QString sql = QueryManager::UPDATE_CUSTOMER.arg(data.customerName).arg(data.customerPhone).arg(data.birthDate.toString("yyyy-MM-dd")).arg(data.gender).arg(data.address).arg(data.memo).arg(data.visitRoot).arg(data.customerId);
        if (!query.exec(sql)) {
            qWarning() << "고객정보 수정 실패:" << query.lastError().text();
            return;
        }
        else {
            qDebug() << "고객정보 수정 성공:";
            QString msg = QString("%1님의 고객정보 수정에 성공 했습니다.").arg(data.customerName);
            QMessageBox::information(this, tr("예약 완료"), msg);
            emit customerUpdateSuccess();
            accept();
        }
    }
    else {
        return;
    }
    
}

void CustomerDetail::onCustomerDeleteBtnClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
        tr("삭제 확인"),      // 창 제목
        tr("%1 고객을 삭제 하시겠습니까?").arg(ui->customerName->text()), // 본문 메시지
        QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
        QSqlQuery query(DBManager::instance().getDatabase());
        QString sql = QStringLiteral(R"(
                        DELETE FROM public."CUSTOMER"
                            WHERE
                               "CUSTOMER_ID" = %1
                    )").arg(data.customerId);
        if (!query.exec(sql)) {
            qWarning() << "고객 삭제 실패:" << query.lastError().text();
            QString msg = QString("%1님의 고객정보 삭제에 실패 했습니다.").arg(data.customerName);
            QMessageBox::information(this, tr("삭제 실패"), msg);
            return;
		}
        else {
            qDebug() << "고객 삭제 성공:";
            QString msg = QString("%1님의 고객정보 삭제에 성공 했습니다.").arg(data.customerName);
            QMessageBox::information(this, tr("삭제 완료"), msg);
            emit customerUpdateSuccess();
            accept();
        }
    }
	else {
		return;
	}
}

void CustomerDetail::loadImages() {
    // DB에서 경로 조회
    QSqlQuery query(DBManager::instance().getDatabase());
    QString sql = QStringLiteral(R"(
                        SELECT
                             "IMAGE_ID",
                             "CUSTOMER_ID",
                             "IMG_PATH",
                             "IMG_NUM"
                        FROM public."IMAGEDATA"
                        WHERE "CUSTOMER_ID" = %1
                        ORDER BY "IMG_NUM" desc
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
        if (index > 9) break;
        QString path = query.value("IMG_PATH").toString();
		imagePaths.append(path);  // 경로 저장
        QPixmap pix(path);
            if (pix.isNull()) {
                qWarning() << "이미지 로드 실패(파일 없음?):" << path;
                continue;
            }
            else {       
                QPixmap scaled = pix.scaled(120, 120,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                // 버튼에 아이콘 세팅
                QIcon icon(scaled);
                imageButtons[index]->setIcon(icon);
                imageButtons[index]->setIconSize(QSize(120, 120));
            }
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

    QString numStr = QString::number(fileList.count()+1);
    QString fn = numStr + QStringLiteral(".") + ext;
    QString dstPath = custDir.filePath(fn);
    if (!QFile::copy(srcFile, dstPath)) {
        qWarning() << "이미지 복사 실패:" << srcFile << "->" << dstPath;
        return;
    }

    // 3) DB에 경로 저장
    QSqlQuery query(DBManager::instance().getDatabase());
    QString sql = QStringLiteral(R"(
                                INSERT INTO public."IMAGEDATA"
                                        ( "CUSTOMER_ID"
                                        , "IMG_PATH"
                                        , "IMG_NUM"
                                        )
                                    VALUES
                                        ( %1    
                                        , '%2'
                                        , %3        
                                        )
                                )").arg(idStr).arg(dstPath).arg(numStr);

    if (!query.exec(sql)) {
        qWarning() << "이미지 DB 저장 실패:" << query.lastError().text();
    }

    loadImages();
}
