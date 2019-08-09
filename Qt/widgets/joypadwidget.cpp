#include "joypadwidget.h"
#include "ui_joypadwidget.h"

JoypadWidget::JoypadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoypadWidget)
{
    ui->setupUi(this);
}

JoypadWidget::~JoypadWidget()
{
    delete ui;
}
