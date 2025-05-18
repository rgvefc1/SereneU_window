

#include <QtWidgets/QApplication>
#include <QtCore/QResource>

#include "SereneU.h"



int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(SereneU);
    QApplication a(argc, argv);
    SereneU w;
    w.show();
    return a.exec();
}
