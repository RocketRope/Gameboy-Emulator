#include "screenwindow.h"
#include "ui_screenwindow.h"

#include <functional>

ScreenWindow::ScreenWindow(Gameboy* _gameboy, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow),
    gameboy(_gameboy),
    scene(this),
    image((uint8*) pixels, 160, 144, QImage::Format_RGB888)
{
    ui->setupUi(this);

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

void ScreenWindow::vblank_update_callback()
{
    gameboy->ppu.get_framebuffer(pixels);
    pixmap->setPixmap(QPixmap::fromImage(image));
}
