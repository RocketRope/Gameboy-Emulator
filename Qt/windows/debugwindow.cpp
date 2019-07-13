#include "debugwindow.h"
#include "ui_debugwindow.h"

#include "basegameboywidget.h"

#include <QFileDialog>
#include <QString>

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
    QString rom_name = QFileDialog::getOpenFileName(this, "Load ROM", last_path, "*.gb");

    last_path = rom_name.left(rom_name.lastIndexOf('/'));

    g_gameboy->load_rom(rom_name.toStdString().data());

    g_gameboy->reset();

    updateChildren();

    this->setWindowTitle("Debug Window | " + rom_name.mid(rom_name.lastIndexOf('/') + 1));
}

void DebugWindow::on_run_to_pushButton_clicked()
{
    bool ok;

    uint16 break_pc = static_cast<uint16>( ui->run_to_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->run(100000, break_pc);

    updateChildren();

}

void DebugWindow::on_run_pushButton_clicked()
{
    g_gameboy->run(100000);

    updateChildren();

}

void DebugWindow::on_step_pushButton_clicked()
{
    g_gameboy->run(1);

    updateChildren();

}

void DebugWindow::on_pushButton_clicked()
{
    g_gameboy->reset();

    updateChildren();

}
