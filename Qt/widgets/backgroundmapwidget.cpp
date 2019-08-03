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

    QPen pen( QApplication::palette().highlight().color() );

    for ( auto& line : view_lines )
    {
        scene.addItem(&line);
        line.setPen(pen);
    }

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

    updateViewLines();

    scene.update();
}

void BackgroundMapWidget::updateViewLines()
{
    const uint8 screen_width  = 160;
    const uint8 screen_height = 144;

    uint8 left = mcu->get_memory_reference(MCU::ADDRESS::SCX) - 1;
    uint8 top  = mcu->get_memory_reference(MCU::ADDRESS::SCY) - 1;

    uint8 bottom = top + screen_height + 2;
    uint8 right  = left + screen_width + 2;

    // Horizontal lines
    if ( left > right )
    {
        // Wrap right size
        view_lines[0].setLine(left, top,    0xFF, top);
        view_lines[2].setLine(left, bottom, 0xFF, bottom);

        view_lines[4].setLine(0x00, top,    right, top);
        view_lines[6].setLine(0x00, bottom, right, bottom);
    }
    else
    {
        view_lines[0].setLine(left, top,    right, top);
        view_lines[2].setLine(left, bottom, right, bottom);

        view_lines[4].setLine(left, top,    right, top);
        view_lines[6].setLine(left, bottom, right, bottom);
    }

    // Vertical lines
    if ( top > bottom )
    {
        // Wrap bottom
        view_lines[1].setLine(right, top, right, 0xFF);
        view_lines[3].setLine(left,  top, left,  0xFF);

        view_lines[5].setLine(right, 0x00, right, bottom);
        view_lines[7].setLine(left,  0x00, left,  bottom);
    }
    else
    {
        view_lines[1].setLine(right, top, right, bottom);
        view_lines[3].setLine(left,  top, left,  bottom);

        view_lines[5].setLine(right, top, right, bottom);
        view_lines[7].setLine(left,  top, left,  bottom);
    }

}
