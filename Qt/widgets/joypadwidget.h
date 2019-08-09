#ifndef JOYPADWIDGET_H
#define JOYPADWIDGET_H

#include <QWidget>

namespace Ui {
class JoypadWidget;
}

class JoypadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoypadWidget(QWidget *parent = nullptr);
    ~JoypadWidget();

private:
    Ui::JoypadWidget *ui;
};

#endif // JOYPADWIDGET_H
