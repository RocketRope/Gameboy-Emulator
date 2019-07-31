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

    uint8 scx = mcu->get_memory_reference(MCU::ADDRESS::SCX);
    uint8 scy = mcu->get_memory_reference(MCU::ADDRESS::SCY);

    scene.addLine(scx, scy, scx + screen_width, scy + screen_height);

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
    /*
    // View lines
    uint8 scx = mcu->get_memory_reference(MCU::Addr::scx);
    uint8 scy = mcu->get_memory_reference(MCU::Addr::scy);

    QPoint p1(scx, scy);
    QPoint p2(scx + screen_width, scy);
    QPoint p3(scx + screen_width, scy + screen_height);
    QPoint p4(scx, scy + screen_height);

    QPoint p5 = p2;
    QPoint p6 = p3;
    QPoint p7 = p4;
    QPoint p8 = p1;

    if ( (scx + screen_width) < scx )
    {
        p5.setX(255);
        p6.setX(255);
        p7.setX(0);
        p8.setX(0);
    }

    if ( (scy + screen_height) < scy )
    {
        p5.setY(0);
        p6.setY(255);
        p7.setY(255);
        p8.setY(0);
    }

    // Horizontal
    view_lines[0].setPoints(p1, p5);
    view_lines[1].setPoints(p2, p8);
    view_lines[2].setPoints(p4, p6);
    view_lines[3].setPoints(p7, p3);

    // Vertical
    view_lines[4].setPoints(p1, p7);
    view_lines[5].setPoints(p4, p8);
    view_lines[6].setPoints(p2, p6);
    view_lines[7].setPoints(p3, p5);

    */

}
