#include "debugwindow.h"
#include "ui_debugwindow.h"

#include "basegameboywidget.h"

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
