#include "cpuregisterswidget.h"
#include "ui_cpuregisterswidget.h"

CpuRegistersWidget::CpuRegistersWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::CpuRegistersWidget)
{
    ui->setupUi(this);

    reg = g_gameboy->get_cpu_registers();

    update();
}

CpuRegistersWidget::~CpuRegistersWidget()
{
    delete ui;
}

void CpuRegistersWidget::update()
{
    reg = g_gameboy->get_cpu_registers();

    ui->af_lineEdit->setText(QString::number(reg.af, 16).toUpper().rightJustified(4, '0'));
    ui->bc_lineEdit->setText(QString::number(reg.bc, 16).toUpper().rightJustified(4, '0'));
    ui->de_lineEdit->setText(QString::number(reg.de, 16).toUpper().rightJustified(4, '0'));
    ui->hl_lineEdit->setText(QString::number(reg.hl, 16).toUpper().rightJustified(4, '0'));
    ui->sp_lineEdit->setText(QString::number(reg.sp, 16).toUpper().rightJustified(4, '0'));
    ui->pc_lineEdit->setText(QString::number(reg.pc, 16).toUpper().rightJustified(4, '0'));

}

// Slots //

void CpuRegistersWidget::on_af_lineEdit_editingFinished()
{
    bool ok;

    reg.af = static_cast<uint16>( ui->af_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}
void CpuRegistersWidget::on_bc_lineEdit_editingFinished()
{
    bool ok;

    reg.bc = static_cast<uint16>( ui->bc_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}
void CpuRegistersWidget::on_de_lineEdit_editingFinished()
{
    bool ok;

    reg.de = static_cast<uint16>( ui->de_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}
void CpuRegistersWidget::on_hl_lineEdit_editingFinished()
{
    bool ok;

    reg.hl = static_cast<uint16>( ui->hl_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}
void CpuRegistersWidget::on_sp_lineEdit_editingFinished()
{
    bool ok;

    reg.sp = static_cast<uint16>( ui->sp_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}
void CpuRegistersWidget::on_pc_lineEdit_editingFinished()
{
    bool ok;

    reg.pc = static_cast<uint16>( ui->pc_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        g_gameboy->set_cpu_registers(reg);

    updateSiblings();
}

