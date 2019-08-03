#ifndef TILEMAPWIDGET_H
#define TILEMAPWIDGET_H

#include "basegameboywidget.h"
#include "tilegraphicsitem.h"

#include <QGraphicsScene>

namespace Ui {
class TileMapWidget;
}

class TileMapWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:

    explicit TileMapWidget(QWidget *parent = nullptr);
    ~TileMapWidget();

    void update();

private:

    Ui::TileMapWidget *ui;

    QGraphicsScene scene;

    std::array<TileGraphicsItem, 768> tiles;
};

#endif // TILEMAPWIDGET_H
