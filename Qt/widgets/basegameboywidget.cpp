#include "basegameboywidget.h"
#include "ui_basegameboywidget.h"

BaseGameboyWidget::BaseGameboyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseGameboyWidget)
{
    ui->setupUi(this);

    connect(this, SIGNAL(updateSiblings()), parent->parent(), SLOT(updateChildren()));
}

BaseGameboyWidget::~BaseGameboyWidget()
{
    delete ui;
}
