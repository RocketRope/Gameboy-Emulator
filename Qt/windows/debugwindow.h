#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QMainWindow>

#include "gameboy.h"

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebugWindow(Gameboy* _gameboy, QWidget *parent = nullptr);
    ~DebugWindow();

private slots:
    void on_actionLoad_ROM_triggered();

    void on_run_to_pushButton_clicked();

    void on_run_pushButton_clicked();

    void on_step_pushButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::DebugWindow *ui;

    Gameboy* gameboy;

    QString last_path = "../";
};

#endif // DEBUGWINDOW_H
