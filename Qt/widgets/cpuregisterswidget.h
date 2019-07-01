#ifndef CPUREGISTERSWIDGET_H
#define CPUREGISTERSWIDGET_H

#include "basegameboywidget.h"

#include "gameboy.h"

namespace Ui {
class CpuRegistersWidget;
}

class CpuRegistersWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit CpuRegistersWidget(QWidget *parent = nullptr);
    ~CpuRegistersWidget();

    void update();

private slots:

    void on_af_lineEdit_editingFinished();
    void on_bc_lineEdit_editingFinished();
    void on_de_lineEdit_editingFinished();
    void on_hl_lineEdit_editingFinished();
    void on_sp_lineEdit_editingFinished();
    void on_pc_lineEdit_editingFinished();

private:
    Ui::CpuRegistersWidget *ui;

    Registers reg;
};

#endif // CPUREGISTERSWIDGET_H
