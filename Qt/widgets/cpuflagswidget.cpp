#include "cpuflagswidget.h"
#include "ui_cpuflagswidget.h"

CpuFlagsWidget::CpuFlagsWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::CpuFlagsWidget)
{
    ui->setupUi(this);

    update();
}

CpuFlagsWidget::~CpuFlagsWidget()
{
    delete ui;
}

void CpuFlagsWidget::update()
{
    ui->zero_checkBox->setChecked(cpu->get_flag(CPU::Flag::zero));
    ui->subtraction_checkBox->setChecked(cpu->get_flag(CPU::Flag::subtraction));
    ui->half_carry_checkBox->setChecked(cpu->get_flag(CPU::Flag::half_carry));
    ui->carry_checkBox->setChecked(cpu->get_flag(CPU::Flag::carry));
}

void CpuFlagsWidget::on_zero_checkBox_toggled(bool checked)
{
    cpu->set_flag(CPU::Flag::zero, checked);

    emit updateWidgets();
}

void CpuFlagsWidget::on_subtraction_checkBox_toggled(bool checked)
{
    cpu->set_flag(CPU::Flag::subtraction, checked);

    emit updateWidgets();
}

void CpuFlagsWidget::on_half_carry_checkBox_toggled(bool checked)
{
   cpu->set_flag(CPU::Flag::half_carry, checked);

    emit updateWidgets();
}

void CpuFlagsWidget::on_carry_checkBox_toggled(bool checked)
{
    cpu->set_flag(CPU::Flag::carry, checked);

    emit updateWidgets();
}
