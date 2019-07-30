#ifndef TILEGRAPHICSITEM_H
#define TILEGRAPHICSITEM_H

#include "types.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPixmap>

class TileGraphicsItem : public QGraphicsPixmapItem
{
public:
    explicit TileGraphicsItem(const Tile* _tile = nullptr, QGraphicsItem *parent = nullptr);

    void setTilePtr(const Tile* _tile);

    void update();

private:

    const Tile* tile;

    QPixmap pixmap;
    PixelArray pixels;
};
#endif // TILEGRAPHICSITEM_H
