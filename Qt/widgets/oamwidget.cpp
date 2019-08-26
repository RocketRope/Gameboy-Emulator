#include "oamwidget.h"
#include "ui_oamwidget.h"

OAMWidget::OAMWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::OAMWidget)
{
    ui->setupUi(this);

    for ( size_t i = 0 ; i < 40 ; i++ )
        ui->layout->addWidget( new ObjectWidget(&mcu->objects[i], i + 1,  this) );
}

OAMWidget::~OAMWidget()
{
    delete ui;
}
