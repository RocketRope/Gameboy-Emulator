#ifndef INTERRUPTWIDGET_H
#define INTERRUPTWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class InterruptWidget;
}

class InterruptWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit InterruptWidget(QWidget *parent = nullptr);
    ~InterruptWidget();

    void update();

private slots:

    // IME Checkbox
    void on_master_interrupt_enable_checkBox_toggled(bool checked);

    // IE Checkboxes
    void on_ie_vblank_checkBox_toggled(bool checked);
    void on_ie_lcd_checkBox_toggled(bool checked);
    void on_ie_timer_checkBox_toggled(bool checked);
    void on_ie_serial_checkBox_toggled(bool checked);
    void on_ie_joypad_checkBox_toggled(bool checked);

    // IF Checkboxes
    void on_if_vblank_checkBox_toggled(bool checked);
    void on_if_lcd_checkBox_toggled(bool checked);
    void on_if_timer_checkBox_toggled(bool checked);
    void on_if_serial_checkBox_toggled(bool checked);
    void on_if_joypad_checkBox_toggled(bool checked);

private:

    Ui::InterruptWidget *ui;
};

#endif // INTERRUPTWIDGET_H
