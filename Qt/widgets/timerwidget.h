#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class TimerWidget;
}

class TimerWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit TimerWidget(QWidget *parent = nullptr);
    ~TimerWidget();

    void update();

private slots:
    void on_timer_enabled_checkBox_toggled(bool checked);

    void on_tima_speed_comboBox_currentIndexChanged(int index);

    void on_tima_lineEdit_editingFinished();

    void on_tma_lineEdit_editingFinished();

private:
    Ui::TimerWidget *ui;
};

#endif // TIMERWIDGET_H
