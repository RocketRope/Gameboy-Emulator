#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include "basegameboywindow.h"

#include "gameboy.h"

namespace Ui {
class DebugWindow;
}

class DebugWindow : public BaseGameboyWindow
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

private slots:
    void on_actionLoad_ROM_triggered();

    void on_run_to_pushButton_clicked();

    void on_run_pushButton_clicked();

    void on_step_pushButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::DebugWindow *ui;

    QString last_path = "../";
};

#endif // DEBUGWINDOW_H
