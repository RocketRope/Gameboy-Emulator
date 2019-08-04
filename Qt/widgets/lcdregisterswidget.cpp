#include "lcdregisterswidget.h"
#include "ui_lcdregisterswidget.h"

LCDRegistersWidget::LCDRegistersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LCDRegistersWidget)
{
    ui->setupUi(this);
}

LCDRegistersWidget::~LCDRegistersWidget()
{
    delete ui;
}
