#ifndef TILEGRAPHICSITEM_H
#define TILEGRAPHICSITEM_H

#include "gameboy.h"

#include <array>

#include <QGraphicsItem>
#include <QPainter>
#include <QImage>

class TileGraphicsItem : public QGraphicsItem
{
public:

    explicit TileGraphicsItem(unsigned int tile = 0, QGraphicsItem *parent = nullptr);
    ~TileGraphicsItem()
    {

    }
    void setTileNumber(unsigned int tile);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    static void updateAllTiles(Gameboy* system);

private:

    unsigned int tile_number;

    const static unsigned int max_tiles = 768;

    static std::array<QImage, max_tiles> images;
    static std::array<Tile_Pixel_Array, max_tiles> bitmaps;
};
#endif // TILEGRAPHICSITEM_H
