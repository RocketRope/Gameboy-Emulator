#include "screenwindow.h"
#include "ui_screenwindow.h"

#include "joypadwidget.h"

#include <QKeyEvent>

#include <functional>

ScreenWindow::ScreenWindow(Gameboy* _gameboy, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow),
    gameboy(_gameboy),
    scene(this),
    image((uint8*) pixels, 160, 144, QImage::Format_RGB888)
{
    ui->setupUi(this);

    installEventFilter(this);

    gameboy->ppu.register_vblank_callback( std::bind(&ScreenWindow::vblank_update_callback, this) );

    ui->graphicsView->setSceneRect(0,0, 160, 144);
    ui->graphicsView->setScene(&scene);

    pixmap = scene.addPixmap( QPixmap() );

    vblank_update_callback();
}

ScreenWindow::~ScreenWindow()
{
    delete ui;
}

void ScreenWindow::updateFramebuffer()
{
    pixmap->setPixmap(QPixmap::fromImage(image));

    BaseGameboyWidget::updateWidgets();
}

void ScreenWindow::vblank_update_callback()
{
    QMetaObject::invokeMethod(this, "updateFramebuffer", Qt::QueuedConnection);
    gameboy->ppu.get_framebuffer(pixels);
}

bool ScreenWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ( (event->type() == QEvent::KeyPress) || (event->type() == QEvent::KeyRelease) )
        return false;
    else if ( event->type() ==  QEvent::FocusOut )
    {
        std::cout << "Out focus" << std::endl;
        return false;
    }

    return QMainWindow::eventFilter(obj, event);
}

void ScreenWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    if ( event->type() == QEvent::ActivationChange )
    {
        if(this->isActiveWindow())
        {
            // Window received focus
            grabKeyboard();
            gameboy->run();
        }
        else
        {
            // Window lost focus
            releaseKeyboard();
            gameboy->pause();

            BaseGameboyWidget::updateWidgets();
        }
    }
}

bool ScreenWindow::setJoypadKey(int key, bool value)
{
    switch ( key )
    {
        case Qt::Key_A:
            gameboy->mcu.joypad.a = value;
            break;
        case Qt::Key_S:
            gameboy->mcu.joypad.b = value;

            break;
        case Qt::Key_Space:
            gameboy->mcu.joypad.select = value;
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            gameboy->mcu.joypad.start = value;
            break;

        case Qt::Key_Up:
            gameboy->mcu.joypad.up = value;
            break;
        case Qt::Key_Left:
            gameboy->mcu.joypad.left = value;
            break;
        case Qt::Key_Right:
            gameboy->mcu.joypad.right = value;
            break;
        case Qt::Key_Down:
            gameboy->mcu.joypad.down = value;
            break;

        default:
            return false;
    }

    return true;
}

void ScreenWindow::keyPressEvent(QKeyEvent *event)
{
    if ( setJoypadKey(event->key(), true) )
        return;

    QWidget::keyPressEvent(event);
}

void ScreenWindow::keyReleaseEvent(QKeyEvent *event)
{
    if ( setJoypadKey(event->key(), false) )
        return;

    QWidget::keyReleaseEvent(event);
}

