/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnClearSend;
    QCheckBox *chk_send_line;
    QPushButton *sendBt;
    QGroupBox *groupBox_2;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QPushButton *clearBt;
    QCheckBox *chk_rev_time;
    QCheckBox *chk_rev_line;
    QCheckBox *chk_rev_hex;
    QGroupBox *groupBox;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QVBoxLayout *verticalLayout_7;
    QComboBox *serialCb;
    QComboBox *baundrateCb;
    QComboBox *databitCb;
    QComboBox *stopbitCb;
    QComboBox *checkbitCb;
    QVBoxLayout *verticalLayout_8;
    QPushButton *bitSerialCheck;
    QPushButton *openBt;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout_2;
    QSpinBox *txtSendMs;
    QCheckBox *chkTimSend;
    QCheckBox *chk_send_hex;
    QGroupBox *groupBox_4;
    QWidget *layoutWidget3;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout;
    QCheckBox *chk_channel_coding;
    QLabel *label;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_11;
    QComboBox *freqcoding_weishu_cb;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_12;
    QComboBox *freqcoding_jinzhi_cb;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_13;
    QLineEdit *Information_Capacity;
    QTableWidget *tableWidget;
    QWidget *layoutWidget4;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_4;
    QPlainTextEdit *recvEdit;
    QGroupBox *groupBox_5;
    QWidget *layoutWidget5;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *coding_information;
    QWidget *widget;
    QCheckBox *input_dec;
    QCheckBox *input_bit;
    QCheckBox *input_hex;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_3;
    QLineEdit *coding_output;
    QWidget *widget_2;
    QWidget *layoutWidget6;
    QHBoxLayout *horizontalLayout_7;
    QCheckBox *output_bit;
    QCheckBox *output_hex;
    QWidget *layoutWidget7;
    QFormLayout *formLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1213, 949);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(12);
        MainWindow->setFont(font);
        MainWindow->setAutoFillBackground(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        btnClearSend = new QPushButton(centralwidget);
        btnClearSend->setObjectName(QString::fromUtf8("btnClearSend"));
        btnClearSend->setGeometry(QRect(840, 650, 111, 31));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(11);
        btnClearSend->setFont(font1);
        chk_send_line = new QCheckBox(centralwidget);
        chk_send_line->setObjectName(QString::fromUtf8("chk_send_line"));
        chk_send_line->setGeometry(QRect(840, 567, 121, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(10);
        chk_send_line->setFont(font2);
        sendBt = new QPushButton(centralwidget);
        sendBt->setObjectName(QString::fromUtf8("sendBt"));
        sendBt->setGeometry(QRect(840, 609, 111, 31));
        sendBt->setContextMenuPolicy(Qt::NoContextMenu);
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 460, 291, 181));
        groupBox_2->setFont(font1);
        layoutWidget = new QWidget(groupBox_2);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 30, 251, 141));
        layoutWidget->setFont(font1);
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        clearBt = new QPushButton(layoutWidget);
        clearBt->setObjectName(QString::fromUtf8("clearBt"));
        clearBt->setFont(font1);

        gridLayout->addWidget(clearBt, 0, 0, 1, 1);

        chk_rev_time = new QCheckBox(layoutWidget);
        chk_rev_time->setObjectName(QString::fromUtf8("chk_rev_time"));

        gridLayout->addWidget(chk_rev_time, 1, 0, 1, 1);

        chk_rev_line = new QCheckBox(layoutWidget);
        chk_rev_line->setObjectName(QString::fromUtf8("chk_rev_line"));

        gridLayout->addWidget(chk_rev_line, 1, 1, 1, 1);

        chk_rev_hex = new QCheckBox(layoutWidget);
        chk_rev_hex->setObjectName(QString::fromUtf8("chk_rev_hex"));

        gridLayout->addWidget(chk_rev_hex, 0, 1, 1, 1);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 50, 261, 381));
        groupBox->setFont(font2);
        layoutWidget1 = new QWidget(groupBox);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(20, 30, 221, 341));
        verticalLayout_5 = new QVBoxLayout(layoutWidget1);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label_6 = new QLabel(layoutWidget1);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_6->addWidget(label_6);

        label_7 = new QLabel(layoutWidget1);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_6->addWidget(label_7);

        label_8 = new QLabel(layoutWidget1);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout_6->addWidget(label_8);

        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        verticalLayout_6->addWidget(label_9);

        label_10 = new QLabel(layoutWidget1);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout_6->addWidget(label_10);


        horizontalLayout_2->addLayout(verticalLayout_6);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        serialCb = new QComboBox(layoutWidget1);
        serialCb->setObjectName(QString::fromUtf8("serialCb"));

        verticalLayout_7->addWidget(serialCb);

        baundrateCb = new QComboBox(layoutWidget1);
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->addItem(QString());
        baundrateCb->setObjectName(QString::fromUtf8("baundrateCb"));

        verticalLayout_7->addWidget(baundrateCb);

        databitCb = new QComboBox(layoutWidget1);
        databitCb->addItem(QString());
        databitCb->addItem(QString());
        databitCb->addItem(QString());
        databitCb->addItem(QString());
        databitCb->setObjectName(QString::fromUtf8("databitCb"));

        verticalLayout_7->addWidget(databitCb);

        stopbitCb = new QComboBox(layoutWidget1);
        stopbitCb->addItem(QString());
        stopbitCb->addItem(QString());
        stopbitCb->addItem(QString());
        stopbitCb->setObjectName(QString::fromUtf8("stopbitCb"));

        verticalLayout_7->addWidget(stopbitCb);

        checkbitCb = new QComboBox(layoutWidget1);
        checkbitCb->addItem(QString());
        checkbitCb->addItem(QString());
        checkbitCb->addItem(QString());
        checkbitCb->setObjectName(QString::fromUtf8("checkbitCb"));

        verticalLayout_7->addWidget(checkbitCb);


        horizontalLayout_2->addLayout(verticalLayout_7);


        verticalLayout_5->addLayout(horizontalLayout_2);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        bitSerialCheck = new QPushButton(layoutWidget1);
        bitSerialCheck->setObjectName(QString::fromUtf8("bitSerialCheck"));

        verticalLayout_8->addWidget(bitSerialCheck);

        openBt = new QPushButton(layoutWidget1);
        openBt->setObjectName(QString::fromUtf8("openBt"));

        verticalLayout_8->addWidget(openBt);


        verticalLayout_5->addLayout(verticalLayout_8);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 650, 291, 181));
        groupBox_3->setFont(font2);
        layoutWidget2 = new QWidget(groupBox_3);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 30, 251, 131));
        gridLayout_2 = new QGridLayout(layoutWidget2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        txtSendMs = new QSpinBox(layoutWidget2);
        txtSendMs->setObjectName(QString::fromUtf8("txtSendMs"));

        gridLayout_2->addWidget(txtSendMs, 1, 1, 1, 1);

        chkTimSend = new QCheckBox(layoutWidget2);
        chkTimSend->setObjectName(QString::fromUtf8("chkTimSend"));

        gridLayout_2->addWidget(chkTimSend, 1, 0, 1, 1);

        chk_send_hex = new QCheckBox(layoutWidget2);
        chk_send_hex->setObjectName(QString::fromUtf8("chk_send_hex"));

        gridLayout_2->addWidget(chk_send_hex, 0, 0, 1, 1);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(330, 460, 471, 211));
        groupBox_4->setFont(font2);
        layoutWidget3 = new QWidget(groupBox_4);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(20, 30, 431, 178));
        gridLayout_4 = new QGridLayout(layoutWidget3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        chk_channel_coding = new QCheckBox(layoutWidget3);
        chk_channel_coding->setObjectName(QString::fromUtf8("chk_channel_coding"));
        chk_channel_coding->setFont(font2);

        verticalLayout->addWidget(chk_channel_coding);

        label = new QLabel(layoutWidget3);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(8);
        label->setFont(font3);

        verticalLayout->addWidget(label);


        gridLayout_4->addLayout(verticalLayout, 1, 0, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_11 = new QLabel(layoutWidget3);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout->addWidget(label_11);

        freqcoding_weishu_cb = new QComboBox(layoutWidget3);
        freqcoding_weishu_cb->addItem(QString());
        freqcoding_weishu_cb->addItem(QString());
        freqcoding_weishu_cb->addItem(QString());
        freqcoding_weishu_cb->addItem(QString());
        freqcoding_weishu_cb->setObjectName(QString::fromUtf8("freqcoding_weishu_cb"));

        horizontalLayout->addWidget(freqcoding_weishu_cb);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_12 = new QLabel(layoutWidget3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_3->addWidget(label_12);

        freqcoding_jinzhi_cb = new QComboBox(layoutWidget3);
        freqcoding_jinzhi_cb->addItem(QString());
        freqcoding_jinzhi_cb->addItem(QString());
        freqcoding_jinzhi_cb->addItem(QString());
        freqcoding_jinzhi_cb->addItem(QString());
        freqcoding_jinzhi_cb->setObjectName(QString::fromUtf8("freqcoding_jinzhi_cb"));

        horizontalLayout_3->addWidget(freqcoding_jinzhi_cb);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_13 = new QLabel(layoutWidget3);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_4->addWidget(label_13);

        Information_Capacity = new QLineEdit(layoutWidget3);
        Information_Capacity->setObjectName(QString::fromUtf8("Information_Capacity"));
        Information_Capacity->setAlignment(Qt::AlignCenter);
        Information_Capacity->setReadOnly(true);

        horizontalLayout_4->addWidget(Information_Capacity);


        verticalLayout_2->addLayout(horizontalLayout_4);


        gridLayout_4->addLayout(verticalLayout_2, 0, 0, 1, 1);

        tableWidget = new QTableWidget(centralwidget);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidget->rowCount() < 4)
            tableWidget->setRowCount(4);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setItem(0, 0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget->setItem(1, 0, __qtablewidgetitem7);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(780, 180, 381, 241));
        tableWidget->setFont(font1);
        tableWidget->setAutoFillBackground(false);
        tableWidget->setStyleSheet(QString::fromUtf8(""));
        tableWidget->setAlternatingRowColors(true);
        layoutWidget4 = new QWidget(centralwidget);
        layoutWidget4->setObjectName(QString::fromUtf8("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(310, 131, 441, 301));
        verticalLayout_4 = new QVBoxLayout(layoutWidget4);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget4);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        verticalLayout_4->addWidget(label_4);

        recvEdit = new QPlainTextEdit(layoutWidget4);
        recvEdit->setObjectName(QString::fromUtf8("recvEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(recvEdit->sizePolicy().hasHeightForWidth());
        recvEdit->setSizePolicy(sizePolicy1);
        recvEdit->setReadOnly(true);

        verticalLayout_4->addWidget(recvEdit);

        groupBox_5 = new QGroupBox(centralwidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(330, 680, 751, 191));
        groupBox_5->setFont(font2);
        layoutWidget5 = new QWidget(groupBox_5);
        layoutWidget5->setObjectName(QString::fromUtf8("layoutWidget5"));
        layoutWidget5->setGeometry(QRect(30, 30, 701, 151));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget5);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_2 = new QLabel(layoutWidget5);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_5->addWidget(label_2);

        coding_information = new QLineEdit(layoutWidget5);
        coding_information->setObjectName(QString::fromUtf8("coding_information"));

        horizontalLayout_5->addWidget(coding_information);


        verticalLayout_3->addLayout(horizontalLayout_5);

        widget = new QWidget(layoutWidget5);
        widget->setObjectName(QString::fromUtf8("widget"));
        input_dec = new QCheckBox(widget);
        input_dec->setObjectName(QString::fromUtf8("input_dec"));
        input_dec->setGeometry(QRect(121, 10, 105, 24));
        QFont font4;
        font4.setPointSize(10);
        input_dec->setFont(font4);
        input_dec->setChecked(true);
        input_dec->setAutoExclusive(true);
        input_bit = new QCheckBox(widget);
        input_bit->setObjectName(QString::fromUtf8("input_bit"));
        input_bit->setGeometry(QRect(10, 10, 105, 24));
        input_bit->setFont(font4);
        input_bit->setAutoExclusive(true);
        input_hex = new QCheckBox(widget);
        input_hex->setObjectName(QString::fromUtf8("input_hex"));
        input_hex->setGeometry(QRect(232, 10, 121, 24));
        input_hex->setFont(font4);
        input_hex->setAutoExclusive(true);

        verticalLayout_3->addWidget(widget);


        horizontalLayout_6->addLayout(verticalLayout_3);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_3 = new QLabel(layoutWidget5);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_8->addWidget(label_3);

        coding_output = new QLineEdit(layoutWidget5);
        coding_output->setObjectName(QString::fromUtf8("coding_output"));
        coding_output->setReadOnly(true);

        horizontalLayout_8->addWidget(coding_output);


        verticalLayout_9->addLayout(horizontalLayout_8);

        widget_2 = new QWidget(layoutWidget5);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        layoutWidget6 = new QWidget(widget_2);
        layoutWidget6->setObjectName(QString::fromUtf8("layoutWidget6"));
        layoutWidget6->setGeometry(QRect(30, 10, 234, 26));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        output_bit = new QCheckBox(layoutWidget6);
        output_bit->setObjectName(QString::fromUtf8("output_bit"));
        output_bit->setFont(font4);
        output_bit->setChecked(true);
        output_bit->setAutoExclusive(true);

        horizontalLayout_7->addWidget(output_bit);

        output_hex = new QCheckBox(layoutWidget6);
        output_hex->setObjectName(QString::fromUtf8("output_hex"));
        output_hex->setFont(font4);
        output_hex->setAutoExclusive(true);

        horizontalLayout_7->addWidget(output_hex);


        verticalLayout_9->addWidget(widget_2);


        horizontalLayout_6->addLayout(verticalLayout_9);

        layoutWidget7 = new QWidget(centralwidget);
        layoutWidget7->setObjectName(QString::fromUtf8("layoutWidget7"));
        layoutWidget7->setGeometry(QRect(0, 0, 2, 2));
        formLayout = new QFormLayout(layoutWidget7);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        MainWindow->setCentralWidget(centralwidget);
        layoutWidget->raise();
        btnClearSend->raise();
        sendBt->raise();
        groupBox_2->raise();
        groupBox->raise();
        groupBox_3->raise();
        layoutWidget->raise();
        groupBox_4->raise();
        chk_send_line->raise();
        tableWidget->raise();
        groupBox_5->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1213, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        baundrateCb->setCurrentIndex(7);
        databitCb->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\270\212\344\275\215\346\234\272", nullptr));
        MainWindow->setStyleSheet(QString());
        btnClearSend->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\345\217\221\351\200\201", nullptr));
        chk_send_line->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\226\260\350\241\214", nullptr));
        sendBt->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\216\245\346\224\266\350\256\276\347\275\256", nullptr));
        clearBt->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\216\245\346\224\266", nullptr));
        chk_rev_time->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264\346\210\263", nullptr));
        chk_rev_line->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\215\242\350\241\214", nullptr));
        chk_rev_hex->setText(QCoreApplication::translate("MainWindow", "Hex\346\216\245\346\224\266", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\350\256\276\347\275\256", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\346\240\241\351\252\214\344\275\215", nullptr));
        baundrateCb->setItemText(0, QCoreApplication::translate("MainWindow", "1200", nullptr));
        baundrateCb->setItemText(1, QCoreApplication::translate("MainWindow", "2400", nullptr));
        baundrateCb->setItemText(2, QCoreApplication::translate("MainWindow", "4800", nullptr));
        baundrateCb->setItemText(3, QCoreApplication::translate("MainWindow", "9600", nullptr));
        baundrateCb->setItemText(4, QCoreApplication::translate("MainWindow", "19200", nullptr));
        baundrateCb->setItemText(5, QCoreApplication::translate("MainWindow", "38400", nullptr));
        baundrateCb->setItemText(6, QCoreApplication::translate("MainWindow", "57600", nullptr));
        baundrateCb->setItemText(7, QCoreApplication::translate("MainWindow", "115200", nullptr));

        databitCb->setItemText(0, QCoreApplication::translate("MainWindow", "5", nullptr));
        databitCb->setItemText(1, QCoreApplication::translate("MainWindow", "6", nullptr));
        databitCb->setItemText(2, QCoreApplication::translate("MainWindow", "7", nullptr));
        databitCb->setItemText(3, QCoreApplication::translate("MainWindow", "8", nullptr));

        stopbitCb->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        stopbitCb->setItemText(1, QCoreApplication::translate("MainWindow", "1.5", nullptr));
        stopbitCb->setItemText(2, QCoreApplication::translate("MainWindow", "2", nullptr));

        checkbitCb->setItemText(0, QCoreApplication::translate("MainWindow", "none", nullptr));
        checkbitCb->setItemText(1, QCoreApplication::translate("MainWindow", "\345\245\207\346\240\241\351\252\214", nullptr));
        checkbitCb->setItemText(2, QCoreApplication::translate("MainWindow", "\345\201\266\346\240\241\351\252\214", nullptr));

        bitSerialCheck->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\265\213\344\270\262\345\217\243", nullptr));
        openBt->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\350\256\276\347\275\256", nullptr));
        chkTimSend->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\345\217\221\351\200\201", nullptr));
        chk_send_hex->setText(QCoreApplication::translate("MainWindow", "Hex\345\217\221\351\200\201", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201\350\256\276\347\275\256", nullptr));
        chk_channel_coding->setText(QCoreApplication::translate("MainWindow", "\346\230\257\345\220\246\350\277\233\350\241\214\344\277\241\351\201\223\347\274\226\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\344\277\241\351\201\223\347\274\226\347\240\201\346\226\271\345\274\217\357\274\232\346\240\271\346\215\256\344\277\241\346\201\257\351\225\277\345\272\246\350\207\252\345\212\250\345\214\271\351\205\215\345\257\271\345\272\224\351\225\277\345\272\246\347\232\204\346\261\211\346\230\216\347\240\201\347\274\226\347\240\201", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\351\242\221\347\216\207\347\274\226\347\240\201\344\275\215\346\225\260", nullptr));
        freqcoding_weishu_cb->setItemText(0, QCoreApplication::translate("MainWindow", "4", nullptr));
        freqcoding_weishu_cb->setItemText(1, QCoreApplication::translate("MainWindow", "5", nullptr));
        freqcoding_weishu_cb->setItemText(2, QCoreApplication::translate("MainWindow", "6", nullptr));
        freqcoding_weishu_cb->setItemText(3, QCoreApplication::translate("MainWindow", "7", nullptr));

        label_12->setText(QCoreApplication::translate("MainWindow", "\351\242\221\347\216\207\347\274\226\347\240\201\350\277\233\345\210\266", nullptr));
        freqcoding_jinzhi_cb->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        freqcoding_jinzhi_cb->setItemText(1, QCoreApplication::translate("MainWindow", "2", nullptr));
        freqcoding_jinzhi_cb->setItemText(2, QCoreApplication::translate("MainWindow", "3", nullptr));
        freqcoding_jinzhi_cb->setItemText(3, QCoreApplication::translate("MainWindow", "4", nullptr));

        freqcoding_jinzhi_cb->setCurrentText(QCoreApplication::translate("MainWindow", "1", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\344\277\241\346\201\257\345\256\271\351\207\217", nullptr));
        Information_Capacity->setText(QCoreApplication::translate("MainWindow", "1bit", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\351\242\221\347\216\207/Hz", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\347\240\201\345\205\203", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "LED0", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "LED1", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "LED2", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "LED3", nullptr));

        const bool __sortingEnabled = tableWidget->isSortingEnabled();
        tableWidget->setSortingEnabled(false);
        tableWidget->setSortingEnabled(__sortingEnabled);

        label_4->setText(QCoreApplication::translate("MainWindow", "\346\216\245\346\224\266\347\252\227\345\217\243", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201\345\206\205\345\256\271", nullptr));
        coding_information->setText(QString());
        input_dec->setText(QCoreApplication::translate("MainWindow", "\345\215\201\350\277\233\345\210\266", nullptr));
        input_bit->setText(QCoreApplication::translate("MainWindow", "\344\272\214\350\277\233\345\210\266", nullptr));
        input_hex->setText(QCoreApplication::translate("MainWindow", "\345\215\201\345\205\255\350\277\233\345\210\266", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\274\226\347\240\201\347\273\223\346\236\234", nullptr));
        output_bit->setText(QCoreApplication::translate("MainWindow", "\344\272\214\350\277\233\345\210\266", nullptr));
        output_hex->setText(QCoreApplication::translate("MainWindow", "\345\215\201\345\205\255\350\277\233\345\210\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
