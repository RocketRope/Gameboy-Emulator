#include "oamwidget.h"
#include "ui_oamwidget.h"

OAMWidget::OAMWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OAMWidget)
{
    ui->setupUi(this);
}

OAMWidget::~OAMWidget()
{
    delete ui;
}
