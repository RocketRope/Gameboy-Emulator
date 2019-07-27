#include "tilemapwidget.h"
#include "ui_tilemapwidget.h"

#include <QImage>

TileMapWidget::TileMapWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::TileMapWidget),
    tile(&gameboy->mcu, 0xFFFF),
    scene(this)
{
    ui->setupUi(this);

    scene.addItem(&tile);
    scene.setSceneRect(0,0,256,320);

    ui->graphicsView->setScene(&scene);

    update();
}

TileMapWidget::~TileMapWidget()
{
    delete ui;
}

void TileMapWidget::update()
{

}

TileGraphicsItem::TileGraphicsItem(MCU *_mcu, uint16 _address, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    mcu(_mcu),
    address(_address)
{
    pixmap = QPixmap::fromImage( QImage(image_data, 8, 8, QImage::Format_RGB888) );

    pixmap.fill(Qt::red);

    this->setPixmap(pixmap);

    update();
}

void TileGraphicsItem::update()
{


}
