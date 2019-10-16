#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>

#include "gameboy.h"

namespace Ui {
class BaseWindow;
}

class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseWindow(Gameboy* gameboy, QWidget *parent = nullptr);
    ~BaseWindow();

    void loadRom(const QString& filename);

private:
    Ui::BaseWindow *ui;

    Gameboy* system;
};

#endif // BASEWINDOW_H
