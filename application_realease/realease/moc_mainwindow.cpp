/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Serial_Project2/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[442];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 26), // "manual_serialPortReadyRead"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 17), // "on_openBt_clicked"
QT_MOC_LITERAL(4, 57, 17), // "on_sendBt_clicked"
QT_MOC_LITERAL(5, 75, 18), // "on_clearBt_clicked"
QT_MOC_LITERAL(6, 94, 23), // "on_btnClearSend_clicked"
QT_MOC_LITERAL(7, 118, 26), // "on_chkTimSend_stateChanged"
QT_MOC_LITERAL(8, 145, 4), // "arg1"
QT_MOC_LITERAL(9, 150, 25), // "on_btnSerialCheck_clicked"
QT_MOC_LITERAL(10, 176, 34), // "on_chk_channel_coding_stateCh..."
QT_MOC_LITERAL(11, 211, 42), // "on_freqcoding_jinzhi_cb_curre..."
QT_MOC_LITERAL(12, 254, 42), // "on_freqcoding_weishu_cb_curre..."
QT_MOC_LITERAL(13, 297, 20), // "on_input_bit_clicked"
QT_MOC_LITERAL(14, 318, 20), // "on_input_dec_clicked"
QT_MOC_LITERAL(15, 339, 20), // "on_input_hex_clicked"
QT_MOC_LITERAL(16, 360, 37), // "on_coding_information_editing..."
QT_MOC_LITERAL(17, 398, 21), // "on_output_bit_clicked"
QT_MOC_LITERAL(18, 420, 21) // "on_output_hex_clicked"

    },
    "MainWindow\0manual_serialPortReadyRead\0"
    "\0on_openBt_clicked\0on_sendBt_clicked\0"
    "on_clearBt_clicked\0on_btnClearSend_clicked\0"
    "on_chkTimSend_stateChanged\0arg1\0"
    "on_btnSerialCheck_clicked\0"
    "on_chk_channel_coding_stateChanged\0"
    "on_freqcoding_jinzhi_cb_currentTextChanged\0"
    "on_freqcoding_weishu_cb_currentTextChanged\0"
    "on_input_bit_clicked\0on_input_dec_clicked\0"
    "on_input_hex_clicked\0"
    "on_coding_information_editingFinished\0"
    "on_output_bit_clicked\0on_output_hex_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    1,   99,    2, 0x08 /* Private */,
       9,    0,  102,    2, 0x08 /* Private */,
      10,    0,  103,    2, 0x08 /* Private */,
      11,    1,  104,    2, 0x08 /* Private */,
      12,    1,  107,    2, 0x08 /* Private */,
      13,    0,  110,    2, 0x08 /* Private */,
      14,    0,  111,    2, 0x08 /* Private */,
      15,    0,  112,    2, 0x08 /* Private */,
      16,    0,  113,    2, 0x08 /* Private */,
      17,    0,  114,    2, 0x08 /* Private */,
      18,    0,  115,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->manual_serialPortReadyRead(); break;
        case 1: _t->on_openBt_clicked(); break;
        case 2: _t->on_sendBt_clicked(); break;
        case 3: _t->on_clearBt_clicked(); break;
        case 4: _t->on_btnClearSend_clicked(); break;
        case 5: _t->on_chkTimSend_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_btnSerialCheck_clicked(); break;
        case 7: _t->on_chk_channel_coding_stateChanged(); break;
        case 8: _t->on_freqcoding_jinzhi_cb_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->on_freqcoding_weishu_cb_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_input_bit_clicked(); break;
        case 11: _t->on_input_dec_clicked(); break;
        case 12: _t->on_input_hex_clicked(); break;
        case 13: _t->on_coding_information_editingFinished(); break;
        case 14: _t->on_output_bit_clicked(); break;
        case 15: _t->on_output_hex_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
