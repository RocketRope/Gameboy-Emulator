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

    Q_INVOKABLE void updateFramebuffer();
    void vblank_update_callback();

    // Events

    bool eventFilter(QObject *obj, QEvent *event);

    void changeEvent(QEvent *event);

    bool setJoypadKey(int key, bool value);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);


private:

    Ui::ScreenWindow *ui;

    Gameboy* gameboy;

    QGraphicsScene scene;

    QImage image;
    QGraphicsPixmapItem* pixmap;
    RGB_Framebuffer pixels;
};

#endif // SCREENWINDOW_H
