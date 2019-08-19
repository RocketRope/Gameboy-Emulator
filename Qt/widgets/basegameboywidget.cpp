#include "basegameboywidget.h"

// Static member declaration
Gameboy* BaseGameboyWidget::gameboy = nullptr;
CPU*     BaseGameboyWidget::cpu     = nullptr;
MCU*     BaseGameboyWidget::mcu     = nullptr;

QVector<BaseGameboyWidget*> BaseGameboyWidget::widgets;


BaseGameboyWidget::BaseGameboyWidget(QWidget *parent) :
    QWidget(parent)
{
    widgets.push_back(this);
}

BaseGameboyWidget::~BaseGameboyWidget()
{
    widgets.remove( widgets.indexOf(this) );
}

void BaseGameboyWidget::setGameboyPtr(Gameboy *_gameboy)
{
    gameboy = _gameboy;
    cpu     = &gameboy->cpu;
    mcu     = &gameboy->mcu;
}

void BaseGameboyWidget::updateWidgets()
{
    for ( auto w : widgets )
        w->update();
}

QString toHexQString(uint32 value, int width)
{
    return QString::number(value, 16).toUpper().rightJustified(width,'0');
}
