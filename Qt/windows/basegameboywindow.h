#ifndef BASEGAMEBOYWINDOW_H
#define BASEGAMEBOYWINDOW_H

#include <QMainWindow>

namespace Ui {
class BaseGameboyWindow;
}

class BaseGameboyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseGameboyWindow(QWidget *parent = nullptr);
    ~BaseGameboyWindow();

public slots:
    void updateChildren();

private:
    Ui::BaseGameboyWindow *ui;
};

#endif // BASEGAMEBOYWINDOW_H
