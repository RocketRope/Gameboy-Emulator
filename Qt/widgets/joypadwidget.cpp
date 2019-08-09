#include "joypadwidget.h"
#include "ui_joypadwidget.h"

JoypadWidget::JoypadWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::JoypadWidget)
{
    ui->setupUi(this);

    update();
}

JoypadWidget::~JoypadWidget()
{
    delete ui;
}

void JoypadWidget::update()
{
    ui->checkBox_up->setChecked(mcu->joypad.up);
    ui->checkBox_left->setChecked(mcu->joypad.left);
    ui->checkBox_right->setChecked(mcu->joypad.right);
    ui->checkBox_down->setChecked(mcu->joypad.down);

    ui->checkBox_select->setChecked(mcu->joypad.select);
    ui->checkBox_start->setChecked(mcu->joypad.start);

    ui->checkBox_a->setChecked(mcu->joypad.a);
    ui->checkBox_b->setChecked(mcu->joypad.b);
}

void JoypadWidget::on_checkBox_up_toggled(bool checked)
{
    mcu->joypad.up = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_left_toggled(bool checked)
{
    mcu->joypad.left = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_right_toggled(bool checked)
{
    mcu->joypad.right = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_down_toggled(bool checked)
{
    mcu->joypad.down = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_select_toggled(bool checked)
{
    mcu->joypad.select = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_start_toggled(bool checked)
{
    mcu->joypad.start = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_a_toggled(bool checked)
{
    mcu->joypad.a = checked;

    updateWidgets();
}

void JoypadWidget::on_checkBox_b_toggled(bool checked)
{
    mcu->joypad.b = checked;

    updateWidgets();
}
