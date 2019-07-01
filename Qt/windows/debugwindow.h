#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include "basegameboywindow.h"

#include "gameboy.h"

namespace Ui {
class DebugWindow;
}

class DebugWindow : public BaseGameboyWindow
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

private:
    Ui::DebugWindow *ui;
};

#endif // DEBUGWINDOW_H
