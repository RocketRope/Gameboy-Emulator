#include "tilemapwidget.h"
#include "ui_tilemapwidget.h"

TileMapWidget::TileMapWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::TileMapWidget),
    scene(this)
{
    ui->setupUi(this);

    ui->graphicsView->setSceneRect(0,0, 256, 192);
    ui->graphicsView->setScene(&scene);

    unsigned int index = 0;

    for ( int bank = 0 ; bank < 2 ; bank++ )
    {
        for ( int y = 0 ; y < 24 ; y++)
        {
            for ( int x = 0 ; x < 16 ; x++ )
            {
                TileGraphicsItem& tile = tiles[index];

                if ( bank == 0 )
                    tile.setTileNumber( index );
                else
                    tile.setTileNumber( index );

                tile.setPos((x * 8) + (bank * 16 * 8), y * 8);

                scene.addItem(&tile);

                index++;
            }
        }
    }

    update();
}

TileMapWidget::~TileMapWidget()
{
    delete ui;
}

void TileMapWidget::update()
{
    scene.update();
}


