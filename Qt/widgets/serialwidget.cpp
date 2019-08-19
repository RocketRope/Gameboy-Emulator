#include "serialwidget.h"
#include "ui_serialwidget.h"

#include <QScrollBar>
#include <functional>

SerialWidget::SerialWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::SerialWidget)
{
    ui->setupUi(this);

    mcu->register_serial_send_callback( std::bind( &SerialWidget::send_serial_callback,
                                                   this,
                                                   std::placeholders::_1
                                                 )
                                      );
}

SerialWidget::~SerialWidget()
{
    delete ui;
}

void SerialWidget::send_serial_callback(uint8 data)
{
    ui->plainTextEdit->insertPlainText( QString(static_cast<char>(data)) );

    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}
