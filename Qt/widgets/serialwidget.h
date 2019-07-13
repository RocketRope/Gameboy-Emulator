#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "basegameboywidget.h"

namespace Ui {
class SerialWidget;
}

class SerialWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit SerialWidget(QWidget *parent = nullptr);
    ~SerialWidget();

    void send_serial_callback(uint8 data);

private:
    Ui::SerialWidget *ui;
};

#endif // SERIALWIDGET_H
