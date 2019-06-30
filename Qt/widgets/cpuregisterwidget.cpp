#include "cpuregisterwidget.h"
#include "ui_cpuregisterwidget.h"

CpuRegisterWidget::CpuRegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CpuRegisterWidget)
{
    ui->setupUi(this);
}

CpuRegisterWidget::~CpuRegisterWidget()
{
    delete ui;
}

void CpuRegisterWidget::update()
{
    reg = gameboy->get_cpu_registers();

    ui->af_lineEdit->setText(QString::number(reg.af, 16).toUpper());
    ui->bc_lineEdit->setText(QString::number(reg.bc, 16).toUpper());
    ui->de_lineEdit->setText(QString::number(reg.de, 16).toUpper());
    ui->hl_lineEdit->setText(QString::number(reg.hl, 16).toUpper());
    ui->sp_lineEdit->setText(QString::number(reg.sp, 16).toUpper());
    ui->pc_lineEdit->setText(QString::number(reg.pc, 16).toUpper());

    QString str;
}
