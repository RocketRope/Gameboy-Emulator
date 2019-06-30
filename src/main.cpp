#include "debugwindow.h"
#include <QApplication>

#include "gameboy.h"

#include "el/easylogging++.h"
INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);

    el::Configurations conf("log.conf");
    el::Loggers::setDefaultConfigurations(conf, true);

    Gameboy gameboy;

    QApplication a(argc, argv);
    DebugWindow w;
    w.show();

    return a.exec();
}
