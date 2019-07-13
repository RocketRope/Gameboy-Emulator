#include "debugwindow.h"
#include "ui_debugwindow.h"

#include "basegameboywidget.h"

#include <QFileDialog>

DebugWindow::DebugWindow(QWidget *parent) :
    BaseGameboyWindow(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::on_actionLoad_ROM_triggered()
{
    QString rom_name = QFileDialog::getOpenFileName(this, "Load ROM", "../", "*.gb");

    g_gameboy->load_rom(rom_name.toStdString().data());
}
