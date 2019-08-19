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
    ui->zero_checkBox->setChecked(cpu->zero);
    ui->subtraction_checkBox->setChecked(cpu->subtraction);
    ui->half_carry_checkBox->setChecked(cpu->half_carry);
    ui->carry_checkBox->setChecked(cpu->carry);
}

void CpuFlagsWidget::on_zero_checkBox_toggled(bool checked)
{
    cpu->zero = checked;

    emit updateWidgets();
}

void CpuFlagsWidget::on_subtraction_checkBox_toggled(bool checked)
{
    cpu->subtraction = checked;

    emit updateWidgets();
}

void CpuFlagsWidget::on_half_carry_checkBox_toggled(bool checked)
{
   cpu->half_carry = checked;

    emit updateWidgets();
}

void CpuFlagsWidget::on_carry_checkBox_toggled(bool checked)
{
    cpu->carry = checked;

    emit updateWidgets();
}
