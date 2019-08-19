#include "lcdregisterswidget.h"
#include "ui_lcdregisterswidget.h"

LCDRegistersWidget::LCDRegistersWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::LCDRegistersWidget),
    lcdc( mcu->get_memory_reference(MCU::ADDRESS::LCDC) ),
    stat( mcu->get_memory_reference(MCU::ADDRESS::STAT) )
{
    ui->setupUi(this);

    ui->comboBox_object_size->addItem("8x8");
    ui->comboBox_object_size->addItem("8x16");

    ui->comboBox_bg_map->addItem("0x9800");
    ui->comboBox_bg_map->addItem("0x9C00");

    ui->comboBox_bg_tile_area->addItem("0x8800");
    ui->comboBox_bg_tile_area->addItem("0x8000");

    ui->comboBox_window_map->addItem("0x9800");
    ui->comboBox_window_map->addItem("0x9C00");

    update();
}

LCDRegistersWidget::~LCDRegistersWidget()
{
    delete ui;
}

void LCDRegistersWidget::update()
{
    // LCDC

    ui->checkBox_bg_display_on->setChecked( get_bit(0, lcdc));
    ui->checkBox_objects_on->setChecked( get_bit(1, lcdc));

    if ( get_bit(2, lcdc) == false )
        ui->comboBox_object_size->setCurrentIndex(0);
    else
        ui->comboBox_object_size->setCurrentIndex(1);

    if ( get_bit(3, lcdc) == false )
        ui->comboBox_bg_map->setCurrentIndex(0);
    else
        ui->comboBox_bg_map->setCurrentIndex(1);

    if ( get_bit(4, lcdc) == false )
        ui->comboBox_bg_tile_area->setCurrentIndex(0);
    else
        ui->comboBox_bg_tile_area->setCurrentIndex(1);

    ui->checkBox_window_on->setChecked( get_bit(5, lcdc) );

    if ( get_bit(6, lcdc) == false )
        ui->comboBox_window_map->setCurrentIndex(0);
    else
        ui->comboBox_window_map->setCurrentIndex(1);

    ui->checkBox_lcd_stop_flag->setChecked( get_bit(7, lcdc) );

    // STAT

    uint8 mode = stat & 0x03;

    if ( mode == 0 )
        ui->label_mode_value->setText("0: Horizontal Blank");
    else if ( mode == 1 )
        ui->label_mode_value->setText("1: Vertical Blank");
    else if ( mode == 2 )
        ui->label_mode_value->setText("2: OAM Search");
    else // mode == 4
        ui->label_mode_value->setText("3: VRAM Transfer");

    ui->checkBox_lyc_coincidence->setChecked( get_bit(2, stat) );

    ui->checkBox_interrupt_hblank->setChecked( get_bit(3, stat) );
    ui->checkBox_interrupt_vblank->setChecked( get_bit(4, stat) );
    ui->checkBox_interrupt_oam->setChecked( get_bit(5, stat) );
    ui->checkBox_interrupt_ly->setChecked( get_bit(6, stat) );
}

void LCDRegistersWidget::on_checkBox_bg_display_on_toggled(bool checked)
{
    set_bit(0, lcdc, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_objects_on_toggled(bool checked)
{
    set_bit(1, lcdc, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_comboBox_object_size_currentIndexChanged(int index)
{
    set_bit(2, lcdc, index != 0);

    updateWidgets();
}

void LCDRegistersWidget::on_comboBox_bg_map_currentIndexChanged(int index)
{
    set_bit(3, lcdc, index != 0);

    updateWidgets();
}

void LCDRegistersWidget::on_comboBox_bg_tile_area_currentIndexChanged(int index)
{
    set_bit(4, lcdc, index != 0);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_window_on_toggled(bool checked)
{
    set_bit(5, lcdc, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_comboBox_window_map_currentIndexChanged(int index)
{
    set_bit(6, lcdc, index != 0);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_lcd_stop_flag_toggled(bool checked)
{
    set_bit(7, lcdc, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_interrupt_hblank_toggled(bool checked)
{
    set_bit(3, stat, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_interrupt_vblank_toggled(bool checked)
{
    set_bit(4, stat, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_interrupt_oam_toggled(bool checked)
{
    set_bit(5, stat, checked);

    updateWidgets();
}

void LCDRegistersWidget::on_checkBox_interrupt_ly_toggled(bool checked)
{
    set_bit(6, stat, checked);

    updateWidgets();
}
