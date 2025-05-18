#include "DBConnector.h"
#include "ui_dbConnector.h"
#include <DBManager.h>
#include <QMessageBox>
#include <qfile.h>
#include <qxmlstream.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QTextStream>

DBConnector::DBConnector(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DBConnector)
{
	ui->setupUi(this);
	setWindowTitle("DB 연결 관리");
	
	ui->btnTrue->setEnabled(false);

	con = DBManager::instance().loadDBConfig("../config/dbConfig.xml");

	//DBConfig config = DBManager::instance().loadDBConfig("../config/dbConfig.xml");
	ui->dbHost->setText(con.host);
	ui->dbPort->setText(QString::number(con.port));
	ui->dbDatabase->setText(con.database);
	ui->dbUser->setText(con.user);
	ui->dbPassword->setEchoMode(QLineEdit::Password);
	ui->dbPassword->setText(con.password);
	ui->dbPoolMode->setText(con.pool_mode);

	connect(ui->btnFalse, &QPushButton::clicked, this, &QDialog::reject);
	connect(ui->btnTrue, &QPushButton::clicked, this, &DBConnector::setDBConfig);
	connect(ui->btnTest, &QPushButton::clicked, this, &DBConnector::dbConnecting);
}

DBConnector::~DBConnector()
{
	delete ui;
}

void DBConnector::dbConnecting()
{
	if (DBManager::instance().isConnected() == true) {
		DBManager::instance().closeConnection();
	}

	QString host = ui->dbHost->text();
	QString port = ui->dbPort->text();
	QString database = ui->dbDatabase->text();
	QString user = ui->dbUser->text();
	QString password = ui->dbPassword->text();
	QString poolmode = ui->dbPoolMode->text();

	// DB 연결
	if (DBManager::instance().connectToDatabase(host, port.toInt(), database, user, password)) {
		ui->btnTrue->setEnabled(true);
		ui->testLabel->setText("✅ DB 연결 성공!");
	}
	else {
		QString err = DBManager::instance().getLastError();
		QMessageBox::critical(this, "DB연결 실패", "연결에 실패했습니다 :\n" + err);
		ui->testLabel->setText("❌ DB 연결 실패!");
		return;
	}

}

bool DBConnector::setDBConfig()
{
	QFile file("../config/dbConfig.xml");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning() << "❌ XML 파일 열기 실패";
		return false;
	}


	QDomDocument doc;
	QDomDocument::ParseResult result = doc.setContent(&file);
	file.close();

	if (!result) {
		qWarning() << "❌ XML 파싱 실패:"
			<< result.errorMessage
			<< " (line:" << result.errorLine
			<< ", column:" << result.errorColumn << ")";
		return false;
	}

	QString host = ui->dbHost->text();
	QString port = ui->dbPort->text();
	QString database = ui->dbDatabase->text();
	QString user = ui->dbUser->text();
	QString password = ui->dbPassword->text();
	QString poolmode = ui->dbPoolMode->text();

	QDomElement root = doc.documentElement();  // <databaseConfig>
	root.firstChildElement("host").firstChild().setNodeValue(host);
	root.firstChildElement("port").firstChild().setNodeValue(port);
	root.firstChildElement("database").firstChild().setNodeValue(database);
	root.firstChildElement("user").firstChild().setNodeValue(user);
	root.firstChildElement("password").firstChild().setNodeValue(password);
	root.firstChildElement("pool_mode").firstChild().setNodeValue(poolmode);

	// 다시 파일에 저장
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		qWarning() << "❌ XML 파일 저장 실패";
		return false;
	}
	QTextStream stream(&file);
	stream << doc.toString();
	file.close();

	QMessageBox::information(this, "DB정보 저장 완료", "✅ DB 설정이 XML에 성공적으로 수정되었습니다.");
	ui->btnFalse->click();
	return true;
}
