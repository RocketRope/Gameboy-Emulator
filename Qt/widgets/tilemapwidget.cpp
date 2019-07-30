#include "tilemapwidget.h"
#include "ui_tilemapwidget.h"

TileMapWidget::TileMapWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::TileMapWidget),
    scene(this)
{
    ui->setupUi(this);

    size_t index = 0;

    for ( int bank = 0 ; bank < 2 ; bank++ )
    {
        for ( int y = 0 ; y < 24 ; y++)
        {
            for ( int x = 0 ; x < 16 ; x++ )
            {
                TileGraphicsItem& tile = tiles[index];

                if ( bank == 0 )
                    tile.setTilePtr( &mcu->tile_map_0[index] );
                else
                    tile.setTilePtr( &mcu->tile_map_1[index] );

                tile.setPos((x * 8) + (bank * 16 * 8), y * 8);

                scene.addItem(&tile);

                index++;
            }
        }
    }

    ui->graphicsView->setSceneRect(0,0, 256, 192);

    //ui->graphicsView->scale(2.0f, 2.0f);

    ui->graphicsView->setScene(&scene);

    update();
}

TileMapWidget::~TileMapWidget()
{
    delete ui;
}

void TileMapWidget::update()
{
    for ( auto& tile : tiles )
        tile.update();
}


