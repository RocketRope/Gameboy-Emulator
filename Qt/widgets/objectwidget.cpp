#include "objectwidget.h"
#include "ui_objectwidget.h"

ObjectWidget::ObjectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectWidget)
{
    ui->setupUi(this);
}

ObjectWidget::~ObjectWidget()
{
    delete ui;
}
