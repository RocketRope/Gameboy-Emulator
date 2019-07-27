#ifndef VRAMWINDOW_H
#define VRAMWINDOW_H

#include <QMainWindow>

namespace Ui {
class VramWindow;
}

class VramWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VramWindow(QWidget *parent = nullptr);
    ~VramWindow();

private:
    Ui::VramWindow *ui;
};

#endif // VRAMWINDOW_H
