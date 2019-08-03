#include "tilegraphicsitem.h"

#include <iostream>

const QRectF TileGraphicsItem::bounding_rect(0.0, 0.0, 8.0, 8.0);

TileGraphicsItem::TileGraphicsItem(const Tile* _tile, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    tile(_tile),
    image( (uint8*) pixels, 8, 8, QImage::Format_RGB888)
{
    update();
}

void TileGraphicsItem::setTilePtr(const Tile *_tile)
{
    tile = _tile;
}

QRectF TileGraphicsItem::boundingRect() const
{
    return bounding_rect;
}

void TileGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->drawImage(QPoint(0,0), image);
   // painter->drawRect( QRectF(QPoint(8,8), QPoint(16,16)));
}

void TileGraphicsItem::update()
{
    if ( tile != nullptr )
    {
        tile->toRGB(pixels);
    }
}

