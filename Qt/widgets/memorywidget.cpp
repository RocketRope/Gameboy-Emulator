#include "memorywidget.h"
#include "ui_memorywidget.h"

// Memory Table Model Class //

MemoryTableModel::MemoryTableModel( MCU* _mcu, QWidget *parent) :
    QAbstractTableModel(parent),
    mcu(_mcu)
{

}

MemoryTableModel::~MemoryTableModel()
{

}

int MemoryTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return (0xFFFF / data_columns) + 1;
}

int MemoryTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    int columns = data_columns;

    if ( show_ascii )
        columns += 1;

    return columns;
}

QVariant MemoryTableModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() )
    {
        if ( role == Qt::TextAlignmentRole )
        {
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
        }

        if ( role == Qt::DisplayRole )
        {
            uint16 address = static_cast<uint16>( index.row() * data_columns );

            if ( index.column() < data_columns )
            {
                address += index.column();

                uint8 value = mcu->read_8bit(address);

                return QString::number(value, 16).toUpper().rightJustified(2, '0');
            }
            else
            {
                QString str("");

                for ( uint16 i = 0 ; i < data_columns ; i++ )
                {
                    char character = static_cast<char>( mcu->read_8bit(address + i) );

                    if ( character < 0x20 )
                        character = 0x2E;

                    str += character;
                }

                return str;
            }
        }
    }

    return QVariant();
}

bool MemoryTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.isValid() && !value.toString().isEmpty() && ( role == Qt::EditRole ) )
    {
        if ( index.column() < data_columns )
        {
            uint8 data = static_cast<uint8>( value.toString().toUInt(nullptr, 16) );

            uint16 address = static_cast<uint16>( ( index.row() * data_columns ) + index.column() );

            mcu->write_8bit(address, data);

            return  true;
        }
    }

    return false;
}

QVariant MemoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( ( orientation == Qt::Orientation::Vertical ) && ( role == Qt::DisplayRole ) )
    {
        int address = section * data_columns;

        QString header;

        if ( address < 0x4000 )
            header = "ROM Bank 00     ";
        else if ( address < 0x8000)
            header = "ROM Bank 01     ";
        else if ( address < 0x8000)
            header = "ROM Bank 01     ";
        else if ( address < 0xA000)
            header = "VRAM Bank 0     ";
        else if ( address < 0xC000)
            header = "Ext RAM Bank 00 ";
        else if ( address < 0xD000)
            header = "WRAM Bank 0     ";
        else if ( address < 0xE000)
            header = "WRAM Bank 1     ";
        else if ( address < 0xFE00)
            header = "ECHO            ";
        else if ( address < 0xFEA0)
            header = "OAM             ";
        else if ( address < 0xFF00)
            header = "                ";
        else if ( address < 0xFF80)
            header = "I/O Registers   ";
        else
            header = "High RAM        ";

        header += "0x";
        header += QString::number(address, 16).toUpper().rightJustified(4, '0');
        header += ":";

        return header;
    }

    return QVariant();
}

Qt::ItemFlags MemoryTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    if ( index.column() < data_columns )
        flag |= Qt::ItemIsEditable;
    else
        flag ^= Qt::ItemIsSelectable;

    return flag;
}

void MemoryTableModel::columnSet(int columns)
{
    int diff = data_columns - columns;

    if ( diff > 0 )
        insertColumns(data_columns, diff);
    else if ( diff < 0 )
        removeColumns(data_columns, diff);

    data_columns = columns;
}

// Memory Widget Class //

MemoryWidget::MemoryWidget(QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::MemoryWidget),
    table_model(mcu, this)
{
    ui->setupUi(this);

    ui->memory_tableView->setModel(&table_model);
    ui->memory_tableView->setColumnWidth(table_model.columnCount(QModelIndex()) - 1, 100);

    ui->size_comboBox->addItem("8");
}

MemoryWidget::~MemoryWidget()
{
    delete ui;
}

void MemoryWidget::update()
{
    ui->memory_tableView->viewport()->update();
}

void MemoryWidget::selectAddress(uint16 address)
{
    int row = address / 8;
    int column = address - (row * 8);

    QModelIndex index = table_model.index(row, column);

    ui->memory_tableView->setCurrentIndex(index);

    update();
}


void MemoryWidget::on_size_comboBox_currentIndexChanged(const QString &/*arg1*/)
{
    update();
}

void MemoryWidget::on_ascii_checkBox_toggled(bool checked)
{
    if ( checked )
        ui->memory_tableView->showColumn(8);
    else
        ui->memory_tableView->hideColumn(8);

    update();
}

void MemoryWidget::on_goto_lineEdit_returnPressed()
{
    bool ok = true;

    uint16 address = static_cast<uint16>( ui->goto_lineEdit->text().toUInt(&ok, 16) );

    if ( ok )
        selectAddress(address);
}
