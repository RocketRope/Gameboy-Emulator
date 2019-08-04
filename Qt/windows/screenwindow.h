#ifndef SCREENWINDOW_H
#define SCREENWINDOW_H

#include <QMainWindow>

namespace Ui {
class ScreenWindow;
}

class ScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenWindow(QWidget *parent = nullptr);
    ~ScreenWindow();

private:
    Ui::ScreenWindow *ui;
};

#endif // SCREENWINDOW_H
