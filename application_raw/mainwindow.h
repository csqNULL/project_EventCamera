#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QString>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSerialPort *serialPort;//定义串口指针
    uint8_t flag_channel_coding = 0;    // 是否进行信道编码
    uint8_t flag_input_pattern = 1;     // 编码输入形式，默认十进制
    uint8_t flag_output_pattern = 0;    // 编码输出形式，默认二进制
    // 0：二进制    1：十进制   2：十六机制
    int information_capacity = 256;     // 信息容量，默认256bit
    int coding_information_num;         // 用户输入的信息
    // 4进制的频率编码码表
    int frequency_list_3[12] = {1000, 1350, 1700, 2000, 2350, 2700, 3000, 3350, 3700, 4000, 4350, 4700};
    int frequency_list_2[8] = {1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500};
    int frequency_list_4[16] = {1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000, 4250, 4500, 4750};
    int frequency_list_1[4] = {1000, 2000, 3000, 4000};
    int end_flag = 0xff;
    int led_index = 0x70;
private slots:

    /*手动连接槽函数*/
    void manual_serialPortReadyRead();

    /*以下为mainwindow.ui文件中点击“转到槽”自动生成的函数*/
    void on_openBt_clicked();

    void on_sendBt_clicked();

    void on_clearBt_clicked();

    void on_btnClearSend_clicked();

    void on_chkTimSend_stateChanged(int arg1);

    void on_btnSerialCheck_clicked();

    //void on_chk_rev_hex_2_stateChanged(int arg1);

    void on_chk_channel_coding_stateChanged();

    void on_freqcoding_jinzhi_cb_currentTextChanged(const QString &arg1);

    void on_freqcoding_weishu_cb_currentTextChanged(const QString &arg1);

    void on_input_bit_clicked();

    void on_input_dec_clicked();

    void on_input_hex_clicked();

    void on_coding_information_editingFinished();

    void on_output_bit_clicked();

    void on_output_hex_clicked();

private:
    Ui::MainWindow *ui;

    // 发送、接收字节计数
    long sendNum, recvNum;
    QLabel *lblSendNum;
    QLabel *lblRecvNum;
    QLabel *lblPortState;
    void setNumOnLabel(QLabel *lbl, QString strS, long num);

    int JudCodingOverflow();
    bool JudCodingBeyond();
    void CalculateCapacity();
    void EditTableSize();
    void EditTableData();
    void DelayMSec(unsigned int msec);
    // 定时发送-定时器
    QTimer *timSend;
    QTimer *timInput;
    QTimer *timSendCommand;
    //QTimer *timCheckPort;
};
#endif // MAINWINDOW_H

