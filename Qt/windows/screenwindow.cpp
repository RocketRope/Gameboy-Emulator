#include "screenwindow.h"
#include "ui_screenwindow.h"

ScreenWindow::ScreenWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow)
{
    ui->setupUi(this);
}

ScreenWindow::~ScreenWindow()
{
    delete ui;
}
