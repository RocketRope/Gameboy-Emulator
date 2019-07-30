#ifndef BACKGROUNDMAPWIDGET_H
#define BACKGROUNDMAPWIDGET_H

#include "basegameboywidget.h"
#include "tilegraphicsitem.h"

#include <QLineF>
#include <QGraphicsScene>


namespace Ui {
class BackgroundMapWidget;
}

class BackgroundMapWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit BackgroundMapWidget(QWidget *parent = nullptr);
    ~BackgroundMapWidget();

    void update();

private:
    const uint8 screen_width  = 160;
    const uint8 screen_height = 144;

    Ui::BackgroundMapWidget *ui;

    QGraphicsScene scene;

    std::vector<QLineF> view_lines;

    std::array<TileGraphicsItem, 1024> tiles;

};

#endif // BACKGROUNDMAPWIDGET_H
