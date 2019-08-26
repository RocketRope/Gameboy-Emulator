#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>

namespace Ui {
class BaseWindow;
}

class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow();

private:
    Ui::BaseWindow *ui;
};

#endif // BASEWINDOW_H
