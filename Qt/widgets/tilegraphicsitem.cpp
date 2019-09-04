#include "tilegraphicsitem.h"

std::array<QImage, TileGraphicsItem::max_tiles> TileGraphicsItem::images;
std::array<Tile_Pixel_Array, TileGraphicsItem::max_tiles> TileGraphicsItem::bitmaps;

TileGraphicsItem::TileGraphicsItem(unsigned int tile, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setTileNumber(tile);
}

void TileGraphicsItem::setTileNumber(unsigned int tile)
{
    if ( tile >= max_tiles)
        tile = max_tiles;

    tile_number = tile;
}


QRectF TileGraphicsItem::boundingRect() const
{
    return QRectF(0.0, 0.0, 8.0, 8.0);
}

void TileGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->drawImage(QPoint(0,0), images[tile_number]);
}

void TileGraphicsItem::updateAllTiles(Gameboy* system)
{
    for ( unsigned int i = 0 ; i < max_tiles ; i++ )
    {
        if ( i < (max_tiles / 2) )
            system->mcu.tile_map_0[i].to_rgb(bitmaps[i]);
        else
            system->mcu.tile_map_1[i].to_rgb(bitmaps[i]);

        images[i] = QImage((uchar*) bitmaps[i], 8, 8, QImage::Format_RGB888);
    }
}

