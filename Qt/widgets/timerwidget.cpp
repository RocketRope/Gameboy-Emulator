#include "timerwidget.h"
#include "ui_timerwidget.h"

TimerWidget::TimerWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::TimerWidget)
{
    ui->setupUi(this);

    ui->tima_speed_comboBox->addItem("00 : 4,096 KHz");
    ui->tima_speed_comboBox->addItem("01 : 262,144 KHz");
    ui->tima_speed_comboBox->addItem("10 : 65,536 KHz");
    ui->tima_speed_comboBox->addItem("11 : 16,384 KHz");

    update();
}

TimerWidget::~TimerWidget()
{
    delete ui;
}

void TimerWidget::update()
{
    uint8 tac = mcu->read_8bit(MCU::Addr::tac);

    ui->timer_enabled_checkBox->setChecked( get_bit(2, tac) );

    ui->tima_speed_comboBox->setCurrentIndex( tac & 0x03 );

    ui->tima_lineEdit->setText( QString::number(mcu->read_8bit(MCU::Addr::tima), 16).toUpper().rightJustified(2, '0') );
    ui->tma_lineEdit->setText( QString::number(mcu->read_8bit(MCU::Addr::tma), 16).toUpper().rightJustified(2, '0') );

    ui->div_low_lineEdit->setText( QString::number(mcu->read_8bit(MCU::Addr::div - 1), 16).toUpper().rightJustified(2, '0'));
    ui->div_high_lineEdit->setText( QString::number(mcu->read_8bit(MCU::Addr::div), 16).toUpper().rightJustified(2, '0'));
}

void TimerWidget::on_timer_enabled_checkBox_toggled(bool checked)
{
    uint8 tac = mcu->read_8bit(MCU::Addr::tac);

    set_bit(2, tac, checked);

    mcu->write_8bit(MCU::Addr::tac, tac);

    updateWidgets();
}

void TimerWidget::on_tima_speed_comboBox_currentIndexChanged(int index)
{
    uint8 tac = mcu->read_8bit(MCU::Addr::tac);

    tac = (tac & 0xFC) | index;

    mcu->write_8bit(MCU::Addr::tac, tac);

    updateWidgets();
}

void TimerWidget::on_tima_lineEdit_editingFinished()
{
    bool ok;

    uint8 tima = static_cast<uint8>( ui->tima_lineEdit->text().toUInt(&ok, 16) );

    mcu->write_8bit(MCU::Addr::tima, tima);

    updateWidgets();
}

void TimerWidget::on_tma_lineEdit_editingFinished()
{
    bool ok;

    uint8 tma = static_cast<uint8>( ui->tma_lineEdit->text().toUInt(&ok, 16) );

    mcu->write_8bit(MCU::Addr::tma, tma);

    updateWidgets();
}
