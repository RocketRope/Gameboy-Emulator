#include "backgroundmapwidget.h"
#include "ui_backgroundmapwidget.h"

BackgroundMapWidget::BackgroundMapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BackgroundMapWidget)
{
    ui->setupUi(this);
}

BackgroundMapWidget::~BackgroundMapWidget()
{
    delete ui;
}
