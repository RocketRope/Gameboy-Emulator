#include "cpuregisterswidget.h"
#include "ui_cpuregisterswidget.h"

CpuRegistersWidget::CpuRegistersWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::CpuRegistersWidget)
{
    ui->setupUi(this);

    update();
}

CpuRegistersWidget::~CpuRegistersWidget()
{
    delete ui;
}

void CpuRegistersWidget::update()
{
    ui->af_lineEdit->setText(QString::number(cpu->reg.af, 16).toUpper().rightJustified(4, '0'));
    ui->bc_lineEdit->setText(QString::number(cpu->reg.bc, 16).toUpper().rightJustified(4, '0'));
    ui->de_lineEdit->setText(QString::number(cpu->reg.de, 16).toUpper().rightJustified(4, '0'));
    ui->hl_lineEdit->setText(QString::number(cpu->reg.hl, 16).toUpper().rightJustified(4, '0'));
    ui->sp_lineEdit->setText(QString::number(cpu->reg.sp, 16).toUpper().rightJustified(4, '0'));
    ui->pc_lineEdit->setText(QString::number(cpu->reg.pc, 16).toUpper().rightJustified(4, '0'));
}

// Slots //

void CpuRegistersWidget::on_af_lineEdit_editingFinished()
{
    bool ok;

    uint16 af = static_cast<uint16>( ui->af_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.af = af;

    updateWidgets();
}
void CpuRegistersWidget::on_bc_lineEdit_editingFinished()
{
    bool ok;

    uint16 bc = static_cast<uint16>( ui->bc_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.bc = bc;

    updateWidgets();
}
void CpuRegistersWidget::on_de_lineEdit_editingFinished()
{
    bool ok;

    uint16 de = static_cast<uint16>( ui->de_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.de = de;

    updateWidgets();
}
void CpuRegistersWidget::on_hl_lineEdit_editingFinished()
{
    bool ok;

    uint16 hl = static_cast<uint16>( ui->hl_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.hl = hl;

    updateWidgets();
}
void CpuRegistersWidget::on_sp_lineEdit_editingFinished()
{
    bool ok;

    uint16 sp = static_cast<uint16>( ui->sp_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.sp = sp;

    updateWidgets();
}
void CpuRegistersWidget::on_pc_lineEdit_editingFinished()
{
    bool ok;

    uint16 pc = static_cast<uint16>( ui->pc_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        cpu->reg.pc = pc;

    updateWidgets();
}

