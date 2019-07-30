#include "tilegraphicsitem.h"

#include <iostream>

TileGraphicsItem::TileGraphicsItem(const Tile* _tile, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent),
    tile(_tile),
    pixmap(8,8)
{
    update();
}

void TileGraphicsItem::setTilePtr(const Tile *_tile)
{
    tile = _tile;
}

void TileGraphicsItem::update()
{
    if ( tile != nullptr )
    {
        pixels = tile->toRGB();

        pixmap = QPixmap::fromImage( QImage( (uint8*) pixels.data(), 8, 8, QImage::Format_RGB888 ) );

        this->setPixmap(pixmap);
    }
}

