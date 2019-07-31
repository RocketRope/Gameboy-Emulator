#include "interruptwidget.h"
#include "ui_interruptwidget.h"

InterruptWidget::InterruptWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::InterruptWidget)
{
    ui->setupUi(this);

    update();
}

InterruptWidget::~InterruptWidget()
{
    delete ui;
}

void InterruptWidget::update()
{
    //IME
    ui->master_interrupt_enable_checkBox->setChecked(cpu->interrupt_enable);

    // IE
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    ui->ie_vblank_checkBox->setChecked( get_bit(0, ie) );
    ui->ie_lcd_checkBox->setChecked( get_bit(1, ie) );
    ui->ie_timer_checkBox->setChecked( get_bit(2, ie) );
    ui->ie_serial_checkBox->setChecked( get_bit(3, ie) );
    ui->ie_joypad_checkBox->setChecked( get_bit(4, ie) );

    // IF
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    ui->if_vblank_checkBox->setChecked( get_bit(0, if_) );
    ui->if_lcd_checkBox->setChecked( get_bit(1, if_) );
    ui->if_timer_checkBox->setChecked( get_bit(2, if_) );
    ui->if_serial_checkBox->setChecked( get_bit(3, if_) );
    ui->if_joypad_checkBox->setChecked( get_bit(4, if_) );
}

// IME Checkbox //
void InterruptWidget::on_master_interrupt_enable_checkBox_toggled(bool checked)
{
    cpu->interrupt_enable = checked;
}

// IE CheckBoxes //
void InterruptWidget::on_ie_vblank_checkBox_toggled(bool checked)
{
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    set_bit(0, ie, checked);

    mcu->write_8bit(MCU::ADDRESS::IE, ie);

    updateWidgets();
}
void InterruptWidget::on_ie_lcd_checkBox_toggled(bool checked)
{
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    set_bit(1, ie, checked);

    mcu->write_8bit(MCU::ADDRESS::IE, ie);

    updateWidgets();
}

void InterruptWidget::on_ie_timer_checkBox_toggled(bool checked)
{
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    set_bit(2, ie, checked);

    mcu->write_8bit(MCU::ADDRESS::IE, ie);

    updateWidgets();
}
void InterruptWidget::on_ie_serial_checkBox_toggled(bool checked)
{
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    set_bit(3, ie, checked);

    mcu->write_8bit(MCU::ADDRESS::IE, ie);

    updateWidgets();
}
void InterruptWidget::on_ie_joypad_checkBox_toggled(bool checked)
{
    uint8 ie = mcu->read_8bit(MCU::ADDRESS::IE);

    set_bit(4, ie, checked);

    mcu->write_8bit(MCU::ADDRESS::IE, ie);

    updateWidgets();
}

// IF Checkboxes //
void InterruptWidget::on_if_vblank_checkBox_toggled(bool checked)
{
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    set_bit(0, if_, checked);

    mcu->write_8bit(MCU::ADDRESS::IF, if_);

    updateWidgets();
}
void InterruptWidget::on_if_lcd_checkBox_toggled(bool checked)
{
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    set_bit(1, if_, checked);

    mcu->write_8bit(MCU::ADDRESS::IF, if_);

    updateWidgets();
}
void InterruptWidget::on_if_timer_checkBox_toggled(bool checked)
{
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    set_bit(2, if_, checked);

    mcu->write_8bit(MCU::ADDRESS::IF, if_);

    updateWidgets();
}
void InterruptWidget::on_if_serial_checkBox_toggled(bool checked)
{
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    set_bit(3, if_, checked);

    mcu->write_8bit(MCU::ADDRESS::IF, if_);

    updateWidgets();
}
void InterruptWidget::on_if_joypad_checkBox_toggled(bool checked)
{
    uint8 if_ = mcu->read_8bit(MCU::ADDRESS::IF);

    set_bit(4, if_, checked);

    mcu->write_8bit(MCU::ADDRESS::IF, if_);

    updateWidgets();
}


