#ifndef CPUREGISTERWIDGET_H
#define CPUREGISTERWIDGET_H

#include <QWidget>

#include "gameboy.h"

namespace Ui {
class CpuRegisterWidget;
}

class CpuRegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CpuRegisterWidget(QWidget *parent = nullptr);
    ~CpuRegisterWidget();

    void update();

private:
    Ui::CpuRegisterWidget *ui;

    Gameboy* gameboy;
    Registers reg;
};

#endif // CPUREGISTERWIDGET_H
