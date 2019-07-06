#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>

#include "basegameboywidget.h"

namespace Ui {
class MemoryWidget;
class MemoryTableModel;
}

class MemoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit MemoryTableModel(QWidget *parent = nullptr);
    ~MemoryTableModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void columnSet(int columns);

private:

    int data_columns = 8;
    bool show_ascii = true;

};

class MemoryWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit MemoryWidget(QWidget *parent = nullptr);
    ~MemoryWidget();

    void update();

    void selectAddress(uint16 address);

private slots:
    void on_size_comboBox_currentIndexChanged(const QString &arg1);

    void on_ascii_checkBox_toggled(bool checked);

    void on_goto_lineEdit_returnPressed();

private:

    Ui::MemoryWidget *ui;

    MemoryTableModel table_model;
};

#endif // MEMORYWIDGET_H
