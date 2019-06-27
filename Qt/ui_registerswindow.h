/********************************************************************************
** Form generated from reading UI file 'registerswindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERSWINDOW_H
#define UI_REGISTERSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
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
#include <QtWidgets/QMenu>
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
    QAction *actionLoadROM;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QPushButton *step_pushButton;
    QGroupBox *io_registers_groupBox;
    QGridLayout *gridLayout_3;
    QTableView *io_registers_tableView;
    QTableView *memory_tableView;
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
    QCheckBox *zero_checkBox;
    QCheckBox *subtraction_checkBox;
    QCheckBox *half_carry_checkBox;
    QCheckBox *carry_checkBox;
    QCheckBox *interrupt_enable_checkBox;
    QPlainTextEdit *source_plainTextEdit;
    QPushButton *reset_pushButton;
    QFrame *run_frame;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *run_pushButton;
    QLineEdit *pc_break_lineEdit;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *RegistersWindow)
    {
        if (RegistersWindow->objectName().isEmpty())
            RegistersWindow->setObjectName(QString::fromUtf8("RegistersWindow"));
        RegistersWindow->resize(781, 670);
        RegistersWindow->setMaximumSize(QSize(16777215, 800));
        actionLoadROM = new QAction(RegistersWindow);
        actionLoadROM->setObjectName(QString::fromUtf8("actionLoadROM"));
        centralWidget = new QWidget(RegistersWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        step_pushButton = new QPushButton(centralWidget);
        step_pushButton->setObjectName(QString::fromUtf8("step_pushButton"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(step_pushButton->sizePolicy().hasHeightForWidth());
        step_pushButton->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(step_pushButton, 6, 1, 1, 1);

        io_registers_groupBox = new QGroupBox(centralWidget);
        io_registers_groupBox->setObjectName(QString::fromUtf8("io_registers_groupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(io_registers_groupBox->sizePolicy().hasHeightForWidth());
        io_registers_groupBox->setSizePolicy(sizePolicy1);
        gridLayout_3 = new QGridLayout(io_registers_groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        io_registers_tableView = new QTableView(io_registers_groupBox);
        io_registers_tableView->setObjectName(QString::fromUtf8("io_registers_tableView"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(io_registers_tableView->sizePolicy().hasHeightForWidth());
        io_registers_tableView->setSizePolicy(sizePolicy2);
        io_registers_tableView->setMaximumSize(QSize(300, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("Monospace"));
        io_registers_tableView->setFont(font);
        io_registers_tableView->setShowGrid(false);
        io_registers_tableView->horizontalHeader()->setVisible(false);

        gridLayout_3->addWidget(io_registers_tableView, 0, 0, 1, 1);


        gridLayout_2->addWidget(io_registers_groupBox, 2, 1, 1, 1);

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

        gridLayout_2->addWidget(memory_tableView, 2, 0, 9, 1);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy4);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cpu_registers_groupBox = new QGroupBox(frame);
        cpu_registers_groupBox->setObjectName(QString::fromUtf8("cpu_registers_groupBox"));
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(cpu_registers_groupBox->sizePolicy().hasHeightForWidth());
        cpu_registers_groupBox->setSizePolicy(sizePolicy5);
        cpu_registers_groupBox->setMinimumSize(QSize(130, 0));
        cpu_registers_groupBox->setMaximumSize(QSize(130, 16777215));
        cpu_registers_groupBox->setFont(font);
        gridLayout = new QGridLayout(cpu_registers_groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hl_label = new QLabel(cpu_registers_groupBox);
        hl_label->setObjectName(QString::fromUtf8("hl_label"));
        sizePolicy5.setHeightForWidth(hl_label->sizePolicy().hasHeightForWidth());
        hl_label->setSizePolicy(sizePolicy5);
        hl_label->setMaximumSize(QSize(30, 16777215));
        hl_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(hl_label, 3, 0, 1, 1);

        de_label = new QLabel(cpu_registers_groupBox);
        de_label->setObjectName(QString::fromUtf8("de_label"));
        sizePolicy5.setHeightForWidth(de_label->sizePolicy().hasHeightForWidth());
        de_label->setSizePolicy(sizePolicy5);
        de_label->setMaximumSize(QSize(30, 16777215));
        de_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(de_label, 2, 0, 1, 1);

        hl_lineEdit = new QLineEdit(cpu_registers_groupBox);
        hl_lineEdit->setObjectName(QString::fromUtf8("hl_lineEdit"));
        sizePolicy4.setHeightForWidth(hl_lineEdit->sizePolicy().hasHeightForWidth());
        hl_lineEdit->setSizePolicy(sizePolicy4);
        hl_lineEdit->setMinimumSize(QSize(60, 0));
        hl_lineEdit->setMaximumSize(QSize(60, 16777215));
        hl_lineEdit->setFont(font);

        gridLayout->addWidget(hl_lineEdit, 3, 1, 1, 1);

        sp_lineEdit = new QLineEdit(cpu_registers_groupBox);
        sp_lineEdit->setObjectName(QString::fromUtf8("sp_lineEdit"));
        sizePolicy4.setHeightForWidth(sp_lineEdit->sizePolicy().hasHeightForWidth());
        sp_lineEdit->setSizePolicy(sizePolicy4);
        sp_lineEdit->setMinimumSize(QSize(60, 0));
        sp_lineEdit->setMaximumSize(QSize(60, 16777215));
        sp_lineEdit->setFont(font);

        gridLayout->addWidget(sp_lineEdit, 4, 1, 1, 1);

        af_label = new QLabel(cpu_registers_groupBox);
        af_label->setObjectName(QString::fromUtf8("af_label"));
        sizePolicy5.setHeightForWidth(af_label->sizePolicy().hasHeightForWidth());
        af_label->setSizePolicy(sizePolicy5);
        af_label->setMaximumSize(QSize(30, 16777215));
        af_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(af_label, 0, 0, 1, 1);

        de_lineEdit = new QLineEdit(cpu_registers_groupBox);
        de_lineEdit->setObjectName(QString::fromUtf8("de_lineEdit"));
        sizePolicy4.setHeightForWidth(de_lineEdit->sizePolicy().hasHeightForWidth());
        de_lineEdit->setSizePolicy(sizePolicy4);
        de_lineEdit->setMinimumSize(QSize(60, 0));
        de_lineEdit->setMaximumSize(QSize(60, 16777215));
        de_lineEdit->setFont(font);

        gridLayout->addWidget(de_lineEdit, 2, 1, 1, 1);

        pc_lineEdit = new QLineEdit(cpu_registers_groupBox);
        pc_lineEdit->setObjectName(QString::fromUtf8("pc_lineEdit"));
        sizePolicy4.setHeightForWidth(pc_lineEdit->sizePolicy().hasHeightForWidth());
        pc_lineEdit->setSizePolicy(sizePolicy4);
        pc_lineEdit->setMinimumSize(QSize(60, 0));
        pc_lineEdit->setMaximumSize(QSize(60, 16777215));
        pc_lineEdit->setFont(font);

        gridLayout->addWidget(pc_lineEdit, 5, 1, 1, 1);

        af_lineEdit = new QLineEdit(cpu_registers_groupBox);
        af_lineEdit->setObjectName(QString::fromUtf8("af_lineEdit"));
        sizePolicy4.setHeightForWidth(af_lineEdit->sizePolicy().hasHeightForWidth());
        af_lineEdit->setSizePolicy(sizePolicy4);
        af_lineEdit->setMinimumSize(QSize(60, 0));
        af_lineEdit->setMaximumSize(QSize(60, 16777215));
        af_lineEdit->setFont(font);
        af_lineEdit->setAutoFillBackground(false);
        af_lineEdit->setClearButtonEnabled(false);

        gridLayout->addWidget(af_lineEdit, 0, 1, 1, 1);

        pc_label = new QLabel(cpu_registers_groupBox);
        pc_label->setObjectName(QString::fromUtf8("pc_label"));
        sizePolicy5.setHeightForWidth(pc_label->sizePolicy().hasHeightForWidth());
        pc_label->setSizePolicy(sizePolicy5);
        pc_label->setMaximumSize(QSize(30, 16777215));
        pc_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(pc_label, 5, 0, 1, 1);

        bc_lineEdit = new QLineEdit(cpu_registers_groupBox);
        bc_lineEdit->setObjectName(QString::fromUtf8("bc_lineEdit"));
        sizePolicy4.setHeightForWidth(bc_lineEdit->sizePolicy().hasHeightForWidth());
        bc_lineEdit->setSizePolicy(sizePolicy4);
        bc_lineEdit->setMinimumSize(QSize(60, 0));
        bc_lineEdit->setMaximumSize(QSize(60, 16777215));
        bc_lineEdit->setFont(font);

        gridLayout->addWidget(bc_lineEdit, 1, 1, 1, 1);

        bc_label = new QLabel(cpu_registers_groupBox);
        bc_label->setObjectName(QString::fromUtf8("bc_label"));
        sizePolicy5.setHeightForWidth(bc_label->sizePolicy().hasHeightForWidth());
        bc_label->setSizePolicy(sizePolicy5);
        bc_label->setMaximumSize(QSize(30, 16777215));
        bc_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(bc_label, 1, 0, 1, 1);

        sp_label = new QLabel(cpu_registers_groupBox);
        sp_label->setObjectName(QString::fromUtf8("sp_label"));
        sizePolicy5.setHeightForWidth(sp_label->sizePolicy().hasHeightForWidth());
        sp_label->setSizePolicy(sizePolicy5);
        sp_label->setMaximumSize(QSize(30, 16777215));
        sp_label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(sp_label, 4, 0, 1, 1);


        horizontalLayout->addWidget(cpu_registers_groupBox);

        Flags_groupBox = new QGroupBox(frame);
        Flags_groupBox->setObjectName(QString::fromUtf8("Flags_groupBox"));
        sizePolicy5.setHeightForWidth(Flags_groupBox->sizePolicy().hasHeightForWidth());
        Flags_groupBox->setSizePolicy(sizePolicy5);
        Flags_groupBox->setMinimumSize(QSize(160, 0));
        Flags_groupBox->setMaximumSize(QSize(160, 16777215));
        verticalLayout = new QVBoxLayout(Flags_groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        zero_checkBox = new QCheckBox(Flags_groupBox);
        zero_checkBox->setObjectName(QString::fromUtf8("zero_checkBox"));

        verticalLayout->addWidget(zero_checkBox);

        subtraction_checkBox = new QCheckBox(Flags_groupBox);
        subtraction_checkBox->setObjectName(QString::fromUtf8("subtraction_checkBox"));

        verticalLayout->addWidget(subtraction_checkBox);

        half_carry_checkBox = new QCheckBox(Flags_groupBox);
        half_carry_checkBox->setObjectName(QString::fromUtf8("half_carry_checkBox"));

        verticalLayout->addWidget(half_carry_checkBox);

        carry_checkBox = new QCheckBox(Flags_groupBox);
        carry_checkBox->setObjectName(QString::fromUtf8("carry_checkBox"));

        verticalLayout->addWidget(carry_checkBox);

        interrupt_enable_checkBox = new QCheckBox(Flags_groupBox);
        interrupt_enable_checkBox->setObjectName(QString::fromUtf8("interrupt_enable_checkBox"));

        verticalLayout->addWidget(interrupt_enable_checkBox);


        horizontalLayout->addWidget(Flags_groupBox);


        gridLayout_2->addWidget(frame, 0, 1, 2, 1);

        source_plainTextEdit = new QPlainTextEdit(centralWidget);
        source_plainTextEdit->setObjectName(QString::fromUtf8("source_plainTextEdit"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(source_plainTextEdit->sizePolicy().hasHeightForWidth());
        source_plainTextEdit->setSizePolicy(sizePolicy6);
        source_plainTextEdit->setMinimumSize(QSize(0, 260));
        source_plainTextEdit->setMaximumSize(QSize(16777215, 800));
        source_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        source_plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        source_plainTextEdit->setReadOnly(true);

        gridLayout_2->addWidget(source_plainTextEdit, 0, 0, 1, 1);

        reset_pushButton = new QPushButton(centralWidget);
        reset_pushButton->setObjectName(QString::fromUtf8("reset_pushButton"));

        gridLayout_2->addWidget(reset_pushButton, 10, 1, 1, 1);

        run_frame = new QFrame(centralWidget);
        run_frame->setObjectName(QString::fromUtf8("run_frame"));
        sizePolicy5.setHeightForWidth(run_frame->sizePolicy().hasHeightForWidth());
        run_frame->setSizePolicy(sizePolicy5);
        run_frame->setMinimumSize(QSize(320, 0));
        horizontalLayout_2 = new QHBoxLayout(run_frame);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        run_pushButton = new QPushButton(run_frame);
        run_pushButton->setObjectName(QString::fromUtf8("run_pushButton"));

        horizontalLayout_2->addWidget(run_pushButton);

        pc_break_lineEdit = new QLineEdit(run_frame);
        pc_break_lineEdit->setObjectName(QString::fromUtf8("pc_break_lineEdit"));
        pc_break_lineEdit->setMinimumSize(QSize(100, 0));
        pc_break_lineEdit->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(pc_break_lineEdit);


        gridLayout_2->addWidget(run_frame, 4, 1, 1, 1);

        RegistersWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RegistersWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 781, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        RegistersWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoadROM);

        retranslateUi(RegistersWindow);

        QMetaObject::connectSlotsByName(RegistersWindow);
    } // setupUi

    void retranslateUi(QMainWindow *RegistersWindow)
    {
        RegistersWindow->setWindowTitle(QCoreApplication::translate("RegistersWindow", "RegistersWindow", nullptr));
        actionLoadROM->setText(QCoreApplication::translate("RegistersWindow", " Load ROM...", nullptr));
        step_pushButton->setText(QCoreApplication::translate("RegistersWindow", "Step", nullptr));
        io_registers_groupBox->setTitle(QCoreApplication::translate("RegistersWindow", "IO Registers", nullptr));
        cpu_registers_groupBox->setTitle(QCoreApplication::translate("RegistersWindow", "Cpu Registers", nullptr));
        hl_label->setText(QCoreApplication::translate("RegistersWindow", "HL:", nullptr));
        de_label->setText(QCoreApplication::translate("RegistersWindow", "DE:", nullptr));
        af_label->setText(QCoreApplication::translate("RegistersWindow", "AF:", nullptr));
        pc_label->setText(QCoreApplication::translate("RegistersWindow", "PC:", nullptr));
        bc_label->setText(QCoreApplication::translate("RegistersWindow", "BC:", nullptr));
        sp_label->setText(QCoreApplication::translate("RegistersWindow", "SP:", nullptr));
        Flags_groupBox->setTitle(QCoreApplication::translate("RegistersWindow", "Flags", nullptr));
        zero_checkBox->setText(QCoreApplication::translate("RegistersWindow", "Zero", nullptr));
        subtraction_checkBox->setText(QCoreApplication::translate("RegistersWindow", "Subtraction", nullptr));
        half_carry_checkBox->setText(QCoreApplication::translate("RegistersWindow", "Half Carry", nullptr));
        carry_checkBox->setText(QCoreApplication::translate("RegistersWindow", "Carry", nullptr));
        interrupt_enable_checkBox->setText(QCoreApplication::translate("RegistersWindow", "Interrupt Enable", nullptr));
        reset_pushButton->setText(QCoreApplication::translate("RegistersWindow", "Reset", nullptr));
        run_pushButton->setText(QCoreApplication::translate("RegistersWindow", "Run", nullptr));
        menuFile->setTitle(QCoreApplication::translate("RegistersWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegistersWindow: public Ui_RegistersWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERSWINDOW_H
