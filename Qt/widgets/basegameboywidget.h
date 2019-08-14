#ifndef BASEGAMEBOYWIDGET_H
#define BASEGAMEBOYWIDGET_H

#include <QWidget>
#include <QVector>

#include "gameboy.h"

namespace Ui {
class BaseGameboyWidget;
}

class BaseGameboyWidget : public QWidget
{
    Q_OBJECT

public:

    explicit BaseGameboyWidget(QWidget *parent = nullptr);
    ~BaseGameboyWidget();

    static void setGameboyPtr(Gameboy* _gameboy);

    virtual void update() {}
    static void updateWidgets();

    template<typename T>
    static void updateWidgetType()
    {
        for ( auto w : widgets )
        {
            if ( dynamic_cast<T>(w) != nullptr )
                w->update();
        }
    }

protected:

    static Gameboy* gameboy;
    static CPU* cpu;
    static MCU* mcu;

private:

    Ui::BaseGameboyWidget *ui;

    static QVector<BaseGameboyWidget*> widgets;
};

// Helper function
QString toHexQString(uint32 value, int width);

#endif // BASEGAMEBOYWIDGET_H
