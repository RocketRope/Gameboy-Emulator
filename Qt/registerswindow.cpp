#include "registerswindow.h"
#include "ui_registerswindow.h"

RegistersWindow::RegistersWindow(Gameboy& gb, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegistersWindow),
    gameboy(gb),
    memory_model(gb.mcu),
    io_register_model(gb.mcu)
{
    ui->setupUi(this);

    ui->memory_tableView->setModel(&memory_model);
    ui->memory_tableView->show();

    ui->io_registers_tableView->setModel(&io_register_model);

    on_actionLoadROM_triggered();

    update_register_widgets();
}

RegistersWindow::~RegistersWindow()
{
    delete ui;
}

void RegistersWindow::update_register_widgets()
{
    QString temp_hexstring;

    // Cpu Registers
    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.af, 4, 16, QLatin1Char( '0' ));
    ui->af_lineEdit->setText(temp_hexstring);

    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.bc, 4, 16, QLatin1Char( '0' ));
    ui->bc_lineEdit->setText(temp_hexstring);

    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.de, 4, 16, QLatin1Char( '0' ));
    ui->de_lineEdit->setText(temp_hexstring);

    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.hl, 4, 16, QLatin1Char( '0' ));
    ui->hl_lineEdit->setText(temp_hexstring);

    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.sp, 4, 16, QLatin1Char( '0' ));
    ui->sp_lineEdit->setText(temp_hexstring);

    temp_hexstring = QString("0x%1").arg(gameboy.cpu.reg.pc, 4, 16, QLatin1Char( '0' ));
    ui->pc_lineEdit->setText(temp_hexstring);

    // Flags
    ui->carry_checkBox->setChecked(gameboy.cpu.get_flag(LR35902::Flag::carry));

    ui->half_carry_checkBox->setChecked(gameboy.cpu.get_flag(LR35902::Flag::half_carry));

    ui->subtraction_checkBox->setChecked(gameboy.cpu.get_flag(LR35902::Flag::sub));

    ui->zero_checkBox->setChecked(gameboy.cpu.get_flag(LR35902::Flag::zero));

    ui->interrupt_enable_checkBox->setChecked(gameboy.cpu.interrupt_enable);

    //
    ui->memory_tableView->viewport()->update();
    ui->io_registers_tableView->viewport()->update();

    //
    highlight_line(10);
}

void RegistersWindow::highlight_line(int line)
{
    QList<QTextEdit::ExtraSelection> extraSelections;


    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);

    ui->source_plainTextEdit->moveCursor(QTextCursor::MoveOperation(line));
    selection.cursor = ui->source_plainTextEdit->textCursor();
    //selection.cursor.setPosition(line);
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    ui->source_plainTextEdit->setExtraSelections(extraSelections);
}

void RegistersWindow::on_step_pushButton_released()
{
    gameboy.cpu.step();
    update_register_widgets();
}

void RegistersWindow::on_reset_pushButton_released()
{
    gameboy.cpu.reset();
    update_register_widgets();
}


void RegistersWindow::on_af_lineEdit_editingFinished()
{
    gameboy.cpu.reg.af = ui->af_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}
void RegistersWindow::on_bc_lineEdit_editingFinished()
{
    gameboy.cpu.reg.bc = ui->bc_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}
void RegistersWindow::on_de_lineEdit_editingFinished()
{
    gameboy.cpu.reg.de = ui->de_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}
void RegistersWindow::on_hl_lineEdit_editingFinished()
{
    gameboy.cpu.reg.hl = ui->hl_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}
void RegistersWindow::on_sp_lineEdit_editingFinished()
{
    gameboy.cpu.reg.sp = ui->sp_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}
void RegistersWindow::on_pc_lineEdit_editingFinished()
{
    gameboy.cpu.reg.pc = ui->pc_lineEdit->text().toUShort(nullptr, 16);
    update_register_widgets();
}

void RegistersWindow::on_carry_checkBox_toggled(bool checked)
{
    gameboy.cpu.set_flag(LR35902::Flag::carry, checked);
    update_register_widgets();
}
void RegistersWindow::on_half_carry_checkBox_toggled(bool checked)
{
    gameboy.cpu.set_flag(LR35902::Flag::half_carry, checked);
    update_register_widgets();
}
void RegistersWindow::on_subtraction_checkBox_toggled(bool checked)
{
    gameboy.cpu.set_flag(LR35902::Flag::sub, checked);
    update_register_widgets();
}
void RegistersWindow::on_zero_checkBox_toggled(bool checked)
{
    gameboy.cpu.set_flag(LR35902::Flag::zero, checked);
    update_register_widgets();
}
void RegistersWindow::on_interrupt_enable_checkBox_toggled(bool checked)
{
    gameboy.cpu.interrupt_enable = checked;
    update_register_widgets();
}

MemoryTableModel::MemoryTableModel(MCU& _mcu, QWidget *parent) :
    QAbstractTableModel(parent),
    mcu(_mcu)
{

}

MemoryTableModel::~MemoryTableModel()
{

}

int MemoryTableModel::rowCount(const QModelIndex & parent) const
{
    return (0xFFFF / columnCount(parent)) + 1;
}

int MemoryTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 8;
}

QVariant MemoryTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        if (role == Qt::TextAlignmentRole)
        {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else if (role == Qt::DisplayRole)
        {
            uint16 address = static_cast<uint16>( (index.row() * 8) + index.column() );

            QString value = QString("%1").arg(mcu.read_8bit(address), 2, 16, QLatin1Char('0'));
            return value;
        }
    }

    return QVariant();
}

bool MemoryTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.isValid() && !value.toString().isEmpty() && role == Qt::EditRole )
    {
        uint16 address = static_cast<uint16>( (index.row() * 8) + index.column() );
        mcu.write_8bit(address, static_cast<uint8>(value.toString().toUShort(nullptr, 16)));

        return true;
    }

    return false;
}

QVariant MemoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( ( orientation == Qt::Orientation::Vertical ) && ( role == Qt::DisplayRole ) )
    {
        return QString("%1").arg(section * 8, 4, 16, QLatin1Char('0')) +
               "-" +
               QString("%1").arg((section * 8) + 7, 4, 16, QLatin1Char('0')) +
               ":";
    }

    return QVariant();
}

Qt::ItemFlags MemoryTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) |= Qt::ItemIsEditable;
}




IoRegistersListModel::IoRegistersListModel(MCU &_mcu, QWidget *parent) :
    QAbstractTableModel(parent),
    mcu(_mcu)
{

}

IoRegistersListModel::~IoRegistersListModel()
{

}

int IoRegistersListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return addresses.size();
}

int IoRegistersListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant IoRegistersListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        if (role == Qt::TextAlignmentRole)
        {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else if (role == Qt::DisplayRole)
        {
            uint16 address = addresses[index.row()].second;

            return QString("%1").arg(mcu.read_8bit(address), 2, 16, QLatin1Char('0'));
        }
    }

    return QVariant();
}

bool IoRegistersListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.isValid() && !value.toString().isEmpty() && role == Qt::EditRole )
    {
        uint16 address = addresses[index.row()].second;
        mcu.write_8bit(address, static_cast<uint8>(value.toString().toUShort(nullptr, 16)));

        return true;
    }

    return false;
}

QVariant IoRegistersListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( ( orientation == Qt::Orientation::Vertical ) && ( role == Qt::DisplayRole ) )
        return QString("0x%1").arg(addresses[section].second, 4, 16, QLatin1Char('0')) +
                " : " +
                addresses[section].first;

    return QVariant();
}

Qt::ItemFlags IoRegistersListModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) |= Qt::ItemIsEditable;
}


void RegistersWindow::on_actionLoadROM_triggered()
{
    gameboy.reset();

    QString rom_name = QFileDialog::getOpenFileName(this,"Load ROM", ".","Rom (*.gb)");
    gameboy.mcu.loadCartridge(rom_name.toStdString().data());
}


void RegistersWindow::on_run_pushButton_clicked()
{
    uint16 pc = ui->pc_break_lineEdit->text().toUShort(nullptr, 16);
    gameboy.cpu.run(pc);

    update_register_widgets();
}
