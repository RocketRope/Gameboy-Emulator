#ifndef TILEMAPWIDGET_H
#define TILEMAPWIDGET_H

#include "basegameboywidget.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>

namespace Ui {
class TileMapWidget;
}

class TileGraphicsItem : public QGraphicsPixmapItem
{
public:
    explicit TileGraphicsItem(MCU* _mcu, uint16 _address, QGraphicsItem *parent = nullptr);

    void update();

private:
    MCU* mcu;
    uint16 address;
    QPixmap pixmap;

    uint8 image_data[8*8*3];
};

class TileMapWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit TileMapWidget(QWidget *parent = nullptr);
    ~TileMapWidget();

    void update();

private:
    Ui::TileMapWidget *ui;

    TileGraphicsItem tile;
    QGraphicsScene scene;
};

#endif // TILEMAPWIDGET_H
