#ifndef CPUFLAGSWIDGET_H
#define CPUFLAGSWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class CpuFlagsWidget;
}

class CpuFlagsWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit CpuFlagsWidget(QWidget *parent = nullptr);
    ~CpuFlagsWidget();

    void update();

private slots:
    void on_zero_checkBox_toggled(bool checked);
    void on_subtraction_checkBox_toggled(bool checked);
    void on_half_carry_checkBox_toggled(bool checked);
    void on_carry_checkBox_toggled(bool checked);

private:
    Ui::CpuFlagsWidget *ui;
};

#endif // CPUFLAGSWIDGET_H
