#include "vramwindow.h"
#include "ui_vramwindow.h"

VramWindow::VramWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VramWindow)
{
    ui->setupUi(this);
}

VramWindow::~VramWindow()
{
    delete ui;
}
