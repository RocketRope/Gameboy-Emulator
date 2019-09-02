#ifndef TILEGRAPHICSITEM_H
#define TILEGRAPHICSITEM_H

#include "types.h"

#include <QGraphicsItem>
#include <QPainter>
#include <QImage>

#include <iostream>

static int counter = 1;

class TileGraphicsItem : public QGraphicsItem
{
public:

    explicit TileGraphicsItem(const Tile* _tile = nullptr, QGraphicsItem *parent = nullptr);

    void setTilePtr(const Tile* _tile);
    void update();

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

    const Tile* tile;

    QImage image;
    Tile_Pixel_Array pixels;

};
#endif // TILEGRAPHICSITEM_H
