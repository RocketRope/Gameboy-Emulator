#ifndef BACKGROUNDMAPWIDGET_H
#define BACKGROUNDMAPWIDGET_H

#include "basegameboywidget.h"
#include "tilegraphicsitem.h"

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

    Ui::BackgroundMapWidget *ui;

    QGraphicsScene scene;

    std::array<TileGraphicsItem, 1024> tiles;

    std::array<QGraphicsLineItem, 8> view_lines;

    void updateViewLines();
};

#endif // BACKGROUNDMAPWIDGET_H
