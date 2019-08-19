#include "debugwindow.h"
#include "ui_debugwindow.h"

#include "basegameboywidget.h"

#include <QFileDialog>
#include <QString>

DebugWindow::DebugWindow(Gameboy* _gameboy, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWindow),
    gameboy(_gameboy)
{
    ui->setupUi(this);
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::on_actionLoad_ROM_triggered()
{
    QString rom_name = QFileDialog::getOpenFileName(this, "Load ROM", last_path, "*.gb");

    last_path = rom_name.left(rom_name.lastIndexOf('/'));

    gameboy->load_rom(rom_name.toStdString().data());

    gameboy->reset();

    BaseGameboyWidget::updateWidgets();

    this->setWindowTitle("Debug Window | " + rom_name.mid(rom_name.lastIndexOf('/') + 1));

    gameboy->run();
}

