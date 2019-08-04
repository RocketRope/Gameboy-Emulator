#ifndef SCREENWINDOW_H
#define SCREENWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "gameboy.h"

namespace Ui {
class ScreenWindow;
}

class ScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenWindow(Gameboy* _gameboy, QWidget *parent = nullptr);
    ~ScreenWindow();

    void vblank_update_callback();

private:
    Ui::ScreenWindow *ui;

    Gameboy* gameboy;

    QGraphicsScene scene;

    QImage image;
    QGraphicsPixmapItem* pixmap;
    RGB_Framebuffer pixels;
};

#endif // SCREENWINDOW_H
