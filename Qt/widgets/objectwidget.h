#ifndef OBJECTWIDGET_H
#define OBJECTWIDGET_H

#include <QWidget>

namespace Ui {
class ObjectWidget;
}

class ObjectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectWidget(QWidget *parent = nullptr);
    ~ObjectWidget();

private:
    Ui::ObjectWidget *ui;
};

#endif // OBJECTWIDGET_H
