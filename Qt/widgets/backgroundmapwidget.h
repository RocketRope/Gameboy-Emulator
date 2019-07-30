#ifndef BACKGROUNDMAPWIDGET_H
#define BACKGROUNDMAPWIDGET_H

#include <QWidget>

namespace Ui {
class BackgroundMapWidget;
}

class BackgroundMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackgroundMapWidget(QWidget *parent = nullptr);
    ~BackgroundMapWidget();

private:
    Ui::BackgroundMapWidget *ui;
};

#endif // BACKGROUNDMAPWIDGET_H
