#ifndef LCDREGISTERSWIDGET_H
#define LCDREGISTERSWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class LCDRegistersWidget;
}

class LCDRegistersWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit LCDRegistersWidget(QWidget *parent = nullptr);
    ~LCDRegistersWidget();

    void update();

private slots:
    void on_checkBox_bg_display_on_toggled(bool checked);

    void on_checkBox_objects_on_toggled(bool checked);

    void on_comboBox_object_size_currentIndexChanged(int index);

    void on_comboBox_bg_map_currentIndexChanged(int index);

    void on_comboBox_bg_tile_area_currentIndexChanged(int index);

    void on_checkBox_window_on_toggled(bool checked);

    void on_comboBox_window_map_currentIndexChanged(int index);

    void on_checkBox_lcd_stop_flag_toggled(bool checked);

    void on_checkBox_interrupt_hblank_toggled(bool checked);

    void on_checkBox_interrupt_vblank_toggled(bool checked);

    void on_checkBox_interrupt_oam_toggled(bool checked);

    void on_checkBox_interrupt_ly_toggled(bool checked);

private:
    Ui::LCDRegistersWidget *ui;

    uint8& lcdc;
    uint8& stat;
};

#endif // LCDREGISTERSWIDGET_H
