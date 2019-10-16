#include "basewindow.h"
#include "ui_basewindow.h"

BaseWindow::BaseWindow(Gameboy* gameboy, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BaseWindow),
    system(gameboy)
{
    ui->setupUi(this);
}

BaseWindow::~BaseWindow()
{
    delete ui;
}

void BaseWindow::loadRom(const QString &filename)
{
    system->load_rom( filename.toStdString() );
}
