#include "interruptwidget.h"
#include "ui_interruptwidget.h"

InterruptWidget::InterruptWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterruptWidget)
{
    ui->setupUi(this);
}

InterruptWidget::~InterruptWidget()
{
    delete ui;
}
