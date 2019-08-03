#ifndef OAMWIDGET_H
#define OAMWIDGET_H

#include <QWidget>

namespace Ui {
class OAMWidget;
}

class OAMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OAMWidget(QWidget *parent = nullptr);
    ~OAMWidget();

private:
    Ui::OAMWidget *ui;
};

#endif // OAMWIDGET_H
