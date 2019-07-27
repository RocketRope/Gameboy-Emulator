#ifndef VRAMWINDOW_H
#define VRAMWINDOW_H

#include <QMainWindow>

#include "gameboy.h"

namespace Ui {
class VramWindow;
}

class VramWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VramWindow(Gameboy* _gameboy, QWidget *parent = nullptr);
    ~VramWindow();

private:
    Ui::VramWindow *ui;

    Gameboy* gameboy;
};

#endif // VRAMWINDOW_H
