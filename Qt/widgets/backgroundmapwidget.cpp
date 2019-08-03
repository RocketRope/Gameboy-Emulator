#include "backgroundmapwidget.h"
#include "ui_backgroundmapwidget.h"

BackgroundMapWidget::BackgroundMapWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::BackgroundMapWidget),
    scene(this)
{
    ui->setupUi(this);

    // Scene
    scene.setSceneRect(0, 0, 256, 256);
    ui->graphicsView->setScene(&scene);

    // Tiles setup
    size_t index = 0;
    for( int y = 0 ; y < 32 ; y++ )
    {
        for ( int x = 0 ; x < 32 ; x++ )
        {
           tiles[index].setPos(x * 8, y * 8);
           scene.addItem(&tiles[index]);
           index++;
        }
    }

    // View lines

    update();
}

BackgroundMapWidget::~BackgroundMapWidget()
{
    delete ui;
}

void BackgroundMapWidget::update()
{
    // Tiles
    for ( size_t i = 0 ; i < 1024 ; i++ )
    {
        tiles[i].setTilePtr( &mcu->tile_map_0[mcu->bg_map_0[i]] );
        tiles[i].update();
    }

    scene.update();
}
