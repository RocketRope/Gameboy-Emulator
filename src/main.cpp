#include "debugwindow.h"
#include "vramwindow.h"
#include "screenwindow.h"
#include "basegameboywidget.h"
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

    BaseGameboyWidget::setGameboyPtr(&gameboy);

    QApplication a(argc, argv);

    DebugWindow debug_window(&gameboy);
    VramWindow vram_window(&gameboy);
    ScreenWindow screen_window(&gameboy);

    debug_window.show();
    vram_window.show();
    screen_window.show();

    return a.exec();
}
