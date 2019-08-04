#ifndef LCDREGISTERSWIDGET_H
#define LCDREGISTERSWIDGET_H

#include <QWidget>

namespace Ui {
class LCDRegistersWidget;
}

class LCDRegistersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LCDRegistersWidget(QWidget *parent = nullptr);
    ~LCDRegistersWidget();

private:
    Ui::LCDRegistersWidget *ui;
};

#endif // LCDREGISTERSWIDGET_H
