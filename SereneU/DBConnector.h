#pragma once
#ifndef DBCREATOR_H
#define DBCREATOR_H
#include <QDialog>
#include <DBManager.h>

namespace Ui {
    class DBConnector;
}
class DBConnector : public QDialog
{
    Q_OBJECT
public:
    explicit DBConnector(QWidget* parent = nullptr);
    ~DBConnector();
private:
    Ui::DBConnector* ui;
    void dbConnecting();
    bool setDBConfig();
    DBConfig con;
};
#endif

