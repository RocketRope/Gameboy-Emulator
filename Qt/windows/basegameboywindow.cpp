#include "basegameboywindow.h"

#include "basegameboywidget.h"

BaseGameboyWindow::BaseGameboyWindow(QWidget *parent) :
    QMainWindow(parent)
{

}

BaseGameboyWindow::~BaseGameboyWindow()
{

}

void BaseGameboyWindow::updateChildren()
{
    auto widgets = this->findChildren<BaseGameboyWidget*>();

    for ( auto w : widgets )
        w->update();
}
