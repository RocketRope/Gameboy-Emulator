#ifndef REGISTERSWINDOW_H
#define REGISTERSWINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>

#include "gameboy.h"

namespace Ui {
class RegistersWindow;
class MemoryTableModel;
class IoRegistersListModel;
}

class MemoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MemoryTableModel(MCU& mcu, QWidget *parent = nullptr);
    ~MemoryTableModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:

    MCU& mcu;
};


class IoRegistersListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit IoRegistersListModel(MCU& _mcu, QWidget *parent = nullptr);
    ~IoRegistersListModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:

    MCU& mcu;

    QVector<QPair<QString, uint16>> addresses {   // Joypad input register
                                                  {"joyp", 0xFF00},

                                                  // Serial data transfer registers
                                                  {"sb", 0xFF01},
                                                  {"sc", 0xFF02},

                                                  // Timer and divider registers
                                                  {"div" , 0xFF04},
                                                  {"tima", 0xFF05},
                                                  {"tma" , 0xFF06},
                                                  {"tac" , 0xFF07},

                                                  // Sound I/O registers
                                                  {"nr10", 0xFF10},
                                                  {"nr11", 0xFF11},
                                                  {"nr12", 0xFF12},
                                                  {"nr13", 0xFF13},
                                                  {"nr14", 0xFF14},

                                                  {"nr21", 0xFF16},
                                                  {"nr22", 0xFF17},
                                                  {"nr23", 0xFF18},
                                                  {"nr24", 0xFF19},

                                                  {"nr30", 0xFF1A},
                                                  {"nr31", 0xFF1B},
                                                  {"nr32", 0xFF1C},
                                                  {"nr33", 0xFF1D},
                                                  {"nr34", 0xFF1E},

                                                  {"nr41", 0xFF20},
                                                  {"nr42", 0xFF21},
                                                  {"nr43", 0xFF22},
                                                  {"nr44", 0xFF23},

                                                  {"nr50", 0xFF24},
                                                  {"nr51", 0xFF25},
                                                  {"nr52", 0xFF26},

                                                  // Video I/O Registers
                                                  {"lcdc", 0xFF40},
                                                  {"stat", 0xFF41},
                                                  {"scy", 0xFF42},
                                                  {"scx" , 0xFF43},
                                                  {"ly"  , 0xFF44},
                                                  {"lyc" , 0xFF45},
                                                  {"dma" , 0xFF46},
                                                  {"bgp" , 0xFF47},
                                                  {"obp0", 0xFF48},
                                                  {"obp1", 0xFF49},
                                                  {"wy"  , 0xFF4A},
                                                  {"wx"  , 0xFF4B},

                                                  // CGB Video I/O registers
                                                  {"vbk"  , 0xFF4F},
                                                  {"hdma1", 0xFF51},
                                                  {"hdma2", 0xFF52},
                                                  {"hdma3", 0xFF53},
                                                  {"hdma4", 0xFF54},
                                                  {"hdma5", 0xFF55},
                                                  {"bcps" , 0xFF68},
                                                  {"bcpd" , 0xFF69},
                                                  {"ocps" , 0xFF6A},
                                                  {"ocpd" , 0xFF6B},

                                                  // CGB registers
                                                  {"key1", 0xFF4D},
                                                  {"rp"  , 0xFF56},
                                                  {"svbk", 0xFF70},

                                                  // Interrupts
                                                  {"if", 0xFF0F},
                                                  {"ie", 0xFFFF}
                                              };

};

class RegistersWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegistersWindow(Gameboy& gb, QWidget *parent = nullptr);
    ~RegistersWindow();

    void update_register_widgets();

private slots:
    void highlight_line(int line);

    void on_step_pushButton_released();
    void on_reset_pushButton_released();

    void on_af_lineEdit_editingFinished();
    void on_bc_lineEdit_editingFinished();
    void on_de_lineEdit_editingFinished();
    void on_hl_lineEdit_editingFinished();
    void on_sp_lineEdit_editingFinished();
    void on_pc_lineEdit_editingFinished();

    void on_carry_checkBox_toggled(bool checked);
    void on_half_carry_checkBox_toggled(bool checked);
    void on_subtraction_checkBox_toggled(bool checked);
    void on_zero_checkBox_toggled(bool checked);
    void on_interrupt_enable_checkBox_toggled(bool checked);

private:
    Ui::RegistersWindow *ui;

    Gameboy& gameboy;
    MemoryTableModel memory_model;
    IoRegistersListModel io_register_model;
};



#endif // REGISTERSWINDOW_H
