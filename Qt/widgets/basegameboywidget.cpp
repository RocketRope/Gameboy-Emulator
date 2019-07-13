#include "basegameboywidget.h"

BaseGameboyWidget::BaseGameboyWidget(QWidget *parent) :
    QWidget(parent)
{
    connect(this, SIGNAL(updateSiblings()), QWidget::window(), SLOT(updateChildren()));
}

BaseGameboyWidget::~BaseGameboyWidget()
{

}
