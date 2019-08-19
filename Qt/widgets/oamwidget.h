#ifndef OAMWIDGET_H
#define OAMWIDGET_H

#include <vector>

#include "objectwidget.h"

namespace Ui {
class OAMWidget;
}

class OAMWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit OAMWidget(QWidget *parent = nullptr);
    ~OAMWidget();

    void update() {}

private:
    Ui::OAMWidget *ui;

    std::vector<ObjectWidget*> objects;

};

#endif // OAMWIDGET_H
