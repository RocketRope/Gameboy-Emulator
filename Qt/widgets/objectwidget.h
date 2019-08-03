#ifndef OBJECTWIDGET_H
#define OBJECTWIDGET_H

#include <QGraphicsScene>

#include "basegameboywidget.h"
#include "tilegraphicsitem.h"
#include "types.h"

namespace Ui {
class ObjectWidget;
}

class ObjectWidget : public BaseGameboyWidget
{
    Q_OBJECT

public:
    explicit ObjectWidget(Object* _object, unsigned int number, QWidget *parent = nullptr);
    ~ObjectWidget();

    void update();

private slots:
    void on_tile_number_lineEdit_editingFinished();

    void on_position_x_lineEdit_editingFinished();

    void on_position_y_lineEdit_editingFinished();

    void on_flip_x_checkBox_toggled(bool checked);

    void on_flip_y_checkBox_toggled(bool checked);

    void on_priority_comboBox_currentIndexChanged(int index);

    void on_tile_bank_comboBox_currentIndexChanged(int index);

    void on_dmg_palette_comboBox_currentIndexChanged(int index);

    void on_cgb_palette_comboBox_currentIndexChanged(int index);

private:
    Ui::ObjectWidget *ui;

    Object* object;

    TileGraphicsItem tile;
    QGraphicsScene scene;
};

#endif // OBJECTWIDGET_H
