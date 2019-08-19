#include "objectwidget.h"
#include "ui_objectwidget.h"

ObjectWidget::ObjectWidget(Object* _object, unsigned int number, QWidget *parent) :
    BaseGameboyWidget(parent),
    ui(new Ui::ObjectWidget),
    object(_object)
{
    ui->setupUi(this);

    // Groupbox title
    ui->groupBox->setTitle(QString::number(number,10) + " ");

    // GrphicsView
    ui->tile_graphicsView->setSceneRect(0,0, 8,8);
    ui->tile_graphicsView->scale(6.0, 6.0);
    ui->tile_graphicsView->setScene(&scene);


    scene.addItem(&tile);

    //
    ui->priority_comboBox->addItem("Priority to Object");
    ui->priority_comboBox->addItem("Priority to Background");

    //
    ui->tile_bank_comboBox->addItem("VRAM Bank 0");
    ui->tile_bank_comboBox->addItem("VRAM Bank 1");

    ui->tile_bank_comboBox->setEnabled(false);

    //
    ui->dmg_palette_comboBox->addItem("OBP0");
    ui->dmg_palette_comboBox->addItem("OBP1");

    //
    ui->cgb_palette_comboBox->addItem("OBP0");
    ui->cgb_palette_comboBox->addItem("OBP1");
    ui->cgb_palette_comboBox->addItem("OBP2");
    ui->cgb_palette_comboBox->addItem("OBP3");

    ui->cgb_palette_comboBox->addItem("OBP4");
    ui->cgb_palette_comboBox->addItem("OBP5");
    ui->cgb_palette_comboBox->addItem("OBP6");
    ui->cgb_palette_comboBox->addItem("OBP7");

    ui->cgb_palette_comboBox->setEnabled(false);

    update();
}

ObjectWidget::~ObjectWidget()
{
    delete ui;
}

void ObjectWidget::update()
{
    // Tile Graphics View

    /* CGB
    if ( object->get_attribute(Object::ATTRIBUTE::TILE_BANK) )
        tile.setTilePtr(&mcu->tile_map_1[object->tile_number]);
    else
        tile.setTilePtr(&mcu->tile_map_0[object->tile_number]);
    */

    tile.setTilePtr(&mcu->tile_map_0[object->tile_number]);

    tile.update();
    scene.update();

    // Tile Number
    ui->tile_number_lineEdit->setText( toHexQString(object->tile_number, 2) );

    // Tile Address

    QString str_address = "0"; // = QString::number( object->get_attribute(Object::ATTRIBUTE::TILE_BANK) );

    uint16 address = (object->tile_number * sizeof(Tile)) + 0x8000;
    str_address += ':' + toHexQString(address, 4);

    ui->tile_address_value_label->setText(str_address);

    // Position
    ui->position_x_lineEdit->setText(QString::number(object->pos_x));
    ui->position_y_lineEdit->setText(QString::number(object->pos_y));

    // Flip
    ui->flip_x_checkBox->setChecked(object->get_attribute(Object::ATTRIBUTE::FLIP_X));
    ui->flip_y_checkBox->setChecked(object->get_attribute(Object::ATTRIBUTE::FLIP_Y));

    // Priority
    if ( object->get_attribute(Object::ATTRIBUTE::PRIORITY) )
        ui->priority_comboBox->setCurrentIndex(1);
    else
        ui->priority_comboBox->setCurrentIndex(0);

    // Tile Bank
    if ( object->get_attribute(Object::ATTRIBUTE::TILE_BANK) )
        ui->tile_bank_comboBox->setCurrentIndex(1);
    else
        ui->tile_bank_comboBox->setCurrentIndex(0);

    // DMG Palette
    if ( object->get_attribute(Object::ATTRIBUTE::GB_PALETTE) )
        ui->dmg_palette_comboBox->setCurrentIndex(1);
    else
        ui->dmg_palette_comboBox->setCurrentIndex(0);

    // CGB Palette
    ui->cgb_palette_comboBox->setCurrentIndex(object->get_cgb_palette());
}

void ObjectWidget::on_tile_number_lineEdit_editingFinished()
{
    bool ok;
    uint8 value = ui->tile_number_lineEdit->text().toUInt(&ok, 16);

    if ( ok )
        object->tile_number = value;

    updateWidgets();
}

void ObjectWidget::on_position_x_lineEdit_editingFinished()
{
    bool ok;
    uint8 value = ui->position_x_lineEdit->text().toUInt(&ok);

    if ( ok )
        object->pos_x = value;

    updateWidgets();
}

void ObjectWidget::on_position_y_lineEdit_editingFinished()
{
    bool ok;
    uint8 value = ui->position_y_lineEdit->text().toUInt(&ok);

    if ( ok )
        object->pos_y = value;

    updateWidgets();
}

void ObjectWidget::on_flip_x_checkBox_toggled(bool checked)
{
    object->set_attribute(Object::ATTRIBUTE::FLIP_X, checked);

    updateWidgets();
}

void ObjectWidget::on_flip_y_checkBox_toggled(bool checked)
{
    object->set_attribute(Object::ATTRIBUTE::FLIP_Y, checked);

    updateWidgets();
}

void ObjectWidget::on_priority_comboBox_currentIndexChanged(int index)
{
    if ( index == 0 )
        object->set_attribute(Object::ATTRIBUTE::PRIORITY, false);
    else
        object->set_attribute(Object::ATTRIBUTE::PRIORITY, true);

    updateWidgets();
}

void ObjectWidget::on_tile_bank_comboBox_currentIndexChanged(int index)
{
    if ( index == 0 )
        object->set_attribute(Object::ATTRIBUTE::TILE_BANK, false);
    else
        object->set_attribute(Object::ATTRIBUTE::TILE_BANK, true);

    updateWidgets();
}

void ObjectWidget::on_dmg_palette_comboBox_currentIndexChanged(int index)
{
    if ( index == 0 )
        object->set_attribute(Object::ATTRIBUTE::GB_PALETTE, false);
    else
        object->set_attribute(Object::ATTRIBUTE::GB_PALETTE, true);

    updateWidgets();
}

void ObjectWidget::on_cgb_palette_comboBox_currentIndexChanged(int index)
{
    object->set_cgb_palette( static_cast<uint8>( index ) );

    updateWidgets();
}
