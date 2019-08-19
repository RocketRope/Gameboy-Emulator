#ifndef JOYPADWIDGET_H
#define JOYPADWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class JoypadWidget;
}

class JoypadWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit JoypadWidget(QWidget *parent = nullptr);
    ~JoypadWidget();

    void update();

private slots:
    void on_checkBox_up_toggled(bool checked);

    void on_checkBox_left_toggled(bool checked);

    void on_checkBox_right_toggled(bool checked);

    void on_checkBox_down_toggled(bool checked);

    void on_checkBox_select_toggled(bool checked);

    void on_checkBox_start_toggled(bool checked);

    void on_checkBox_a_toggled(bool checked);

    void on_checkBox_b_toggled(bool checked);

private:
    Ui::JoypadWidget *ui;
};

#endif // JOYPADWIDGET_H
