#include "oamwidget.h"
#include "ui_oamwidget.h"

OAMWidget::OAMWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::OAMWidget)
{
    ui->setupUi(this);

    objects.reserve(40);

    for ( size_t i = 0 ; i < 40 ; i++ )
    {
        objects.push_back( new ObjectWidget(&mcu->objects[i], this) );

        ui->layout->addWidget(objects[i]);

        ui->layout->addSpacing(5);
    }

}

OAMWidget::~OAMWidget()
{
    delete ui;

    for ( auto object : objects )
        delete object;
}
