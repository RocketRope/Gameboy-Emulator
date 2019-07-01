#include "basegameboywindow.h"
#include "ui_basegameboywindow.h"

#include "basegameboywidget.h"

BaseGameboyWindow::BaseGameboyWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BaseGameboyWindow)
{
    ui->setupUi(this);
}

BaseGameboyWindow::~BaseGameboyWindow()
{
    delete ui;
}

void BaseGameboyWindow::updateChildren()
{
    auto widgets = this->findChildren<BaseGameboyWidget*>();

    for ( auto w : widgets )
        w->update();
}
