/********************************************************************************
** Form generated from reading UI file 'registerswindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERSWINDOW_H
#define UI_REGISTERSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegistersWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QPlainTextEdit *source_plainTextEdit;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QGroupBox *cpu_registers_groupBox;
    QGridLayout *gridLayout;
    QLabel *hl_label;
    QLabel *de_label;
    QLineEdit *hl_lineEdit;
    QLineEdit *sp_lineEdit;
    QLabel *af_label;
    QLineEdit *de_lineEdit;
    QLineEdit *pc_lineEdit;
    QLineEdit *af_lineEdit;
    QLabel *pc_label;
    QLineEdit *bc_lineEdit;
    QLabel *bc_label;
    QLabel *sp_label;
    QGroupBox *Flags_groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *carry_checkBox;
    QCheckBox *half_carry_checkBox;
    QCheckBox *subtraction_checkBox;
    QCheckBox *zero_checkBox;
    QCheckBox *interrupt_enable_checkBox;
    QTableView *memory_tableView;
    QPushButton *step_pushButton;
    QPushButton *reset_pushButton;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QTableView *io_registers_tableView;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *RegistersWindow)
    {
        if (RegistersWindow->objectName().isEmpty())
            RegistersWindow->setObjectName(QString::fromUtf8("RegistersWindow"));
        RegistersWindow->resize(824, 660);
        RegistersWindow->setMaximumSize(QSize(16777215, 800));
        centralWidget = new QWidget(RegistersWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        source_plainTextEdit = new QPlainTextEdit(centralWidget);
        source_plainTextEdit->setObjectName(QString::fromUtf8("source_plainTextEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(source_plainTextEdit->sizePolicy().hasHeightForWidth());
        source_plainTextEdit->setSizePolicy(sizePolicy);
        source_plainTextEdit->setMinimumSize(QSize(0, 260));
        source_plainTextEdit->setMaximumSize(QSize(16777215, 800));
        source_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        source_plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        source_plainTextEdit->setReadOnly(true);

        gridLayout_2->addWidget(source_plainTextEdit, 0, 0, 1, 1);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cpu_registers_groupBox = new QGroupBox(frame);
        cpu_registers_groupBox->setObjectName(QString::fromUtf8("cpu_registers_groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cpu_registers_groupBox->sizePolicy().hasHeightForWidth());
        cpu_registers_groupBox->setSizePolicy(sizePolicy2);
        cpu_registers_groupBox->setMinimumSize(QSize(130, 0));
        cpu_registers_groupBox->setMaximumSize(QSize(130, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("Monospace"));
        cpu_registers_groupBox->setFont(font);
        gridLayout = new QGridLayout(cpu_registers_groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hl_label = new QLabel(cpu_registers_groupBox);
        hl_label->setObjectName(QString::fromUtf8("hl_label"));
        sizePolicy2.setHeightForWidth(hl_label->sizePolicy().hasHeightForWidth());
        hl_label->setSizePolicy(sizePolicy2);
        hl_label->setMaximumSize(QSize(30, 16777215));
        hl_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(hl_label, 3, 0, 1, 1);

        de_label = new QLabel(cpu_registers_groupBox);
        de_label->setObjectName(QString::fromUtf8("de_label"));
        sizePolicy2.setHeightForWidth(de_label->sizePolicy().hasHeightForWidth());
        de_label->setSizePolicy(sizePolicy2);
        de_label->setMaximumSize(QSize(30, 16777215));
        de_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(de_label, 2, 0, 1, 1);

        hl_lineEdit = new QLineEdit(cpu_registers_groupBox);
        hl_lineEdit->setObjectName(QString::fromUtf8("hl_lineEdit"));
        sizePolicy1.setHeightForWidth(hl_lineEdit->sizePolicy().hasHeightForWidth());
        hl_lineEdit->setSizePolicy(sizePolicy1);
        hl_lineEdit->setMinimumSize(QSize(60, 0));
        hl_lineEdit->setMaximumSize(QSize(60, 16777215));
        hl_lineEdit->setFont(font);

        gridLayout->addWidget(hl_lineEdit, 3, 1, 1, 1);

        sp_lineEdit = new QLineEdit(cpu_registers_groupBox);
        sp_lineEdit->setObjectName(QString::fromUtf8("sp_lineEdit"));
        sizePolicy1.setHeightForWidth(sp_lineEdit->sizePolicy().hasHeightForWidth());
        sp_lineEdit->setSizePolicy(sizePolicy1);
        sp_lineEdit->setMinimumSize(QSize(60, 0));
        sp_lineEdit->setMaximumSize(QSize(60, 16777215));
        sp_lineEdit->setFont(font);

        gridLayout->addWidget(sp_lineEdit, 4, 1, 1, 1);

        af_label = new QLabel(cpu_registers_groupBox);
        af_label->setObjectName(QString::fromUtf8("af_label"));
        sizePolicy2.setHeightForWidth(af_label->sizePolicy().hasHeightForWidth());
        af_label->setSizePolicy(sizePolicy2);
        af_label->setMaximumSize(QSize(30, 16777215));
        af_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(af_label, 0, 0, 1, 1);

        de_lineEdit = new QLineEdit(cpu_registers_groupBox);
        de_lineEdit->setObjectName(QString::fromUtf8("de_lineEdit"));
        sizePolicy1.setHeightForWidth(de_lineEdit->sizePolicy().hasHeightForWidth());
        de_lineEdit->setSizePolicy(sizePolicy1);
        de_lineEdit->setMinimumSize(QSize(60, 0));
        de_lineEdit->setMaximumSize(QSize(60, 16777215));
        de_lineEdit->setFont(font);

        gridLayout->addWidget(de_lineEdit, 2, 1, 1, 1);

        pc_lineEdit = new QLineEdit(cpu_registers_groupBox);
        pc_lineEdit->setObjectName(QString::fromUtf8("pc_lineEdit"));
        sizePolicy1.setHeightForWidth(pc_lineEdit->sizePolicy().hasHeightForWidth());
        pc_lineEdit->setSizePolicy(sizePolicy1);
        pc_lineEdit->setMinimumSize(QSize(60, 0));
        pc_lineEdit->setMaximumSize(QSize(60, 16777215));
        pc_lineEdit->setFont(font);

        gridLayout->addWidget(pc_lineEdit, 5, 1, 1, 1);

        af_lineEdit = new QLineEdit(cpu_registers_groupBox);
        af_lineEdit->setObjectName(QString::fromUtf8("af_lineEdit"));
        sizePolicy1.setHeightForWidth(af_lineEdit->sizePolicy().hasHeightForWidth());
        af_lineEdit->setSizePolicy(sizePolicy1);
        af_lineEdit->setMinimumSize(QSize(60, 0));
        af_lineEdit->setMaximumSize(QSize(60, 16777215));
        af_lineEdit->setFont(font);
        af_lineEdit->setAutoFillBackground(false);
        af_lineEdit->setClearButtonEnabled(false);

        gridLayout->addWidget(af_lineEdit, 0, 1, 1, 1);

        pc_label = new QLabel(cpu_registers_groupBox);
        pc_label->setObjectName(QString::fromUtf8("pc_label"));
        sizePolicy2.setHeightForWidth(pc_label->sizePolicy().hasHeightForWidth());
        pc_label->setSizePolicy(sizePolicy2);
        pc_label->setMaximumSize(QSize(30, 16777215));
        pc_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(pc_label, 5, 0, 1, 1);

        bc_lineEdit = new QLineEdit(cpu_registers_groupBox);
        bc_lineEdit->setObjectName(QString::fromUtf8("bc_lineEdit"));
        sizePolicy1.setHeightForWidth(bc_lineEdit->sizePolicy().hasHeightForWidth());
        bc_lineEdit->setSizePolicy(sizePolicy1);
        bc_lineEdit->setMinimumSize(QSize(60, 0));
        bc_lineEdit->setMaximumSize(QSize(60, 16777215));
        bc_lineEdit->setFont(font);

        gridLayout->addWidget(bc_lineEdit, 1, 1, 1, 1);

        bc_label = new QLabel(cpu_registers_groupBox);
        bc_label->setObjectName(QString::fromUtf8("bc_label"));
        sizePolicy2.setHeightForWidth(bc_label->sizePolicy().hasHeightForWidth());
        bc_label->setSizePolicy(sizePolicy2);
        bc_label->setMaximumSize(QSize(30, 16777215));
        bc_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(bc_label, 1, 0, 1, 1);

        sp_label = new QLabel(cpu_registers_groupBox);
        sp_label->setObjectName(QString::fromUtf8("sp_label"));
        sizePolicy2.setHeightForWidth(sp_label->sizePolicy().hasHeightForWidth());
        sp_label->setSizePolicy(sizePolicy2);
        sp_label->setMaximumSize(QSize(30, 16777215));
        sp_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(sp_label, 4, 0, 1, 1);


        horizontalLayout->addWidget(cpu_registers_groupBox);

        Flags_groupBox = new QGroupBox(frame);
        Flags_groupBox->setObjectName(QString::fromUtf8("Flags_groupBox"));
        sizePolicy2.setHeightForWidth(Flags_groupBox->sizePolicy().hasHeightForWidth());
        Flags_groupBox->setSizePolicy(sizePolicy2);
        Flags_groupBox->setMinimumSize(QSize(160, 0));
        Flags_groupBox->setMaximumSize(QSize(160, 16777215));
        verticalLayout = new QVBoxLayout(Flags_groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        carry_checkBox = new QCheckBox(Flags_groupBox);
        carry_checkBox->setObjectName(QString::fromUtf8("carry_checkBox"));

        verticalLayout->addWidget(carry_checkBox);

        half_carry_checkBox = new QCheckBox(Flags_groupBox);
        half_carry_checkBox->setObjectName(QString::fromUtf8("half_carry_checkBox"));

        verticalLayout->addWidget(half_carry_checkBox);

        subtraction_checkBox = new QCheckBox(Flags_groupBox);
        subtraction_checkBox->setObjectName(QString::fromUtf8("subtraction_checkBox"));

        verticalLayout->addWidget(subtraction_checkBox);

        zero_checkBox = new QCheckBox(Flags_groupBox);
        zero_checkBox->setObjectName(QString::fromUtf8("zero_checkBox"));

        verticalLayout->addWidget(zero_checkBox);

        interrupt_enable_checkBox = new QCheckBox(Flags_groupBox);
        interrupt_enable_checkBox->setObjectName(QString::fromUtf8("interrupt_enable_checkBox"));

        verticalLayout->addWidget(interrupt_enable_checkBox);


        horizontalLayout->addWidget(Flags_groupBox);


        gridLayout_2->addWidget(frame, 0, 1, 2, 1);

        memory_tableView = new QTableView(centralWidget);
        memory_tableView->setObjectName(QString::fromUtf8("memory_tableView"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(memory_tableView->sizePolicy().hasHeightForWidth());
        memory_tableView->setSizePolicy(sizePolicy3);
        memory_tableView->setFont(font);
        memory_tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        memory_tableView->setTextElideMode(Qt::ElideRight);
        memory_tableView->setShowGrid(false);
        memory_tableView->horizontalHeader()->setVisible(false);
        memory_tableView->horizontalHeader()->setMinimumSectionSize(30);
        memory_tableView->horizontalHeader()->setDefaultSectionSize(30);
        memory_tableView->verticalHeader()->setMinimumSectionSize(30);

        gridLayout_2->addWidget(memory_tableView, 2, 0, 5, 1);

        step_pushButton = new QPushButton(centralWidget);
        step_pushButton->setObjectName(QString::fromUtf8("step_pushButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(step_pushButton->sizePolicy().hasHeightForWidth());
        step_pushButton->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(step_pushButton, 5, 1, 1, 1);

        reset_pushButton = new QPushButton(centralWidget);
        reset_pushButton->setObjectName(QString::fromUtf8("reset_pushButton"));

        gridLayout_2->addWidget(reset_pushButton, 6, 1, 1, 1);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy5);
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        io_registers_tableView = new QTableView(groupBox);
        io_registers_tableView->setObjectName(QString::fromUtf8("io_registers_tableView"));
        QSizePolicy sizePolicy6(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(io_registers_tableView->sizePolicy().hasHeightForWidth());
        io_registers_tableView->setSizePolicy(sizePolicy6);
        io_registers_tableView->setMaximumSize(QSize(300, 16777215));
        io_registers_tableView->setFont(font);
        io_registers_tableView->setShowGrid(false);
        io_registers_tableView->horizontalHeader()->setVisible(false);

        gridLayout_3->addWidget(io_registers_tableView, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 2, 1, 1, 1);

        RegistersWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RegistersWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 824, 27));
        RegistersWindow->setMenuBar(menuBar);

        retranslateUi(RegistersWindow);

        QMetaObject::connectSlotsByName(RegistersWindow);
    } // setupUi

    void retranslateUi(QMainWindow *RegistersWindow)
    {
        RegistersWindow->setWindowTitle(QApplication::translate("RegistersWindow", "RegistersWindow", nullptr));
        cpu_registers_groupBox->setTitle(QApplication::translate("RegistersWindow", "Cpu Registers", nullptr));
        hl_label->setText(QApplication::translate("RegistersWindow", "HL:", nullptr));
        de_label->setText(QApplication::translate("RegistersWindow", "DE:", nullptr));
        af_label->setText(QApplication::translate("RegistersWindow", "AF:", nullptr));
        pc_label->setText(QApplication::translate("RegistersWindow", "PC:", nullptr));
        bc_label->setText(QApplication::translate("RegistersWindow", "BC:", nullptr));
        sp_label->setText(QApplication::translate("RegistersWindow", "SP:", nullptr));
        Flags_groupBox->setTitle(QApplication::translate("RegistersWindow", "Flags", nullptr));
        carry_checkBox->setText(QApplication::translate("RegistersWindow", "Carry", nullptr));
        half_carry_checkBox->setText(QApplication::translate("RegistersWindow", "Half Carry", nullptr));
        subtraction_checkBox->setText(QApplication::translate("RegistersWindow", "Subtraction", nullptr));
        zero_checkBox->setText(QApplication::translate("RegistersWindow", "Zero", nullptr));
        interrupt_enable_checkBox->setText(QApplication::translate("RegistersWindow", "Interrupt Enable", nullptr));
        step_pushButton->setText(QApplication::translate("RegistersWindow", "Step", nullptr));
        reset_pushButton->setText(QApplication::translate("RegistersWindow", "Reset", nullptr));
        groupBox->setTitle(QApplication::translate("RegistersWindow", "IO Registers", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegistersWindow: public Ui_RegistersWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERSWINDOW_H
