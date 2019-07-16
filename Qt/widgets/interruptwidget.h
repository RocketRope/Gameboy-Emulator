#ifndef INTERRUPTWIDGET_H
#define INTERRUPTWIDGET_H

#include <QWidget>

namespace Ui {
class InterruptWidget;
}

class InterruptWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InterruptWidget(QWidget *parent = nullptr);
    ~InterruptWidget();

private:
    Ui::InterruptWidget *ui;
};

#endif // INTERRUPTWIDGET_H
