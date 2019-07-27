#include "vramwindow.h"
#include "ui_vramwindow.h"

VramWindow::VramWindow(Gameboy* _gameboy, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VramWindow),
    gameboy(_gameboy)
{
    ui->setupUi(this);
}

VramWindow::~VramWindow()
{
    delete ui;
}
