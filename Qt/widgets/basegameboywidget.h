#ifndef BASEGAMEBOYWIDGET_H
#define BASEGAMEBOYWIDGET_H

#include <QWidget>

#include "gameboy.h"

namespace Ui {
class BaseGameboyWidget;
}

class BaseGameboyWidget : public QWidget
{
    Q_OBJECT

public:

    CPU* cpu = &g_gameboy->cpu;
    MCU* mcu = &g_gameboy->mcu;

    explicit BaseGameboyWidget(QWidget *parent = nullptr);
    ~BaseGameboyWidget();

    virtual void update() = 0;

signals:
    void updateSiblings();

private:
    Ui::BaseGameboyWidget *ui;

};

#endif // BASEGAMEBOYWIDGET_H
