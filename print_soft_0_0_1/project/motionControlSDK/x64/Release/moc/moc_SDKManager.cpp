/****************************************************************************
** Meta object code from reading C++ file 'SDKManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/sdk/SDKManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SDKManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SDKManager_t {
    QByteArrayData data[17];
    char stringdata0[196];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SDKManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SDKManager_t qt_meta_stringdata_SDKManager = {
    {
QT_MOC_LITERAL(0, 0, 10), // "SDKManager"
QT_MOC_LITERAL(1, 11, 10), // "onRecvData"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "data"
QT_MOC_LITERAL(4, 28, 10), // "onTcpError"
QT_MOC_LITERAL(5, 39, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 68, 5), // "error"
QT_MOC_LITERAL(7, 74, 14), // "onStateChanged"
QT_MOC_LITERAL(8, 89, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(9, 118, 5), // "state"
QT_MOC_LITERAL(10, 124, 11), // "onHeartbeat"
QT_MOC_LITERAL(11, 136, 10), // "onCmdReply"
QT_MOC_LITERAL(12, 147, 3), // "cmd"
QT_MOC_LITERAL(13, 151, 7), // "errCode"
QT_MOC_LITERAL(14, 159, 3), // "arr"
QT_MOC_LITERAL(15, 163, 15), // "onSendHeartbeat"
QT_MOC_LITERAL(16, 179, 16) // "onCheckHeartbeat"

    },
    "SDKManager\0onRecvData\0\0data\0onTcpError\0"
    "QAbstractSocket::SocketError\0error\0"
    "onStateChanged\0QAbstractSocket::SocketState\0"
    "state\0onHeartbeat\0onCmdReply\0cmd\0"
    "errCode\0arr\0onSendHeartbeat\0"
    "onCheckHeartbeat"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SDKManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    1,   52,    2, 0x08 /* Private */,
       7,    1,   55,    2, 0x08 /* Private */,
      10,    0,   58,    2, 0x08 /* Private */,
      11,    3,   59,    2, 0x08 /* Private */,
      15,    0,   66,    2, 0x08 /* Private */,
      16,    0,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::UChar, QMetaType::QByteArray,   12,   13,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SDKManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SDKManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onRecvData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->onTcpError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 2: _t->onStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 3: _t->onHeartbeat(); break;
        case 4: _t->onCmdReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uchar(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3]))); break;
        case 5: _t->onSendHeartbeat(); break;
        case 6: _t->onCheckHeartbeat(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SDKManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_SDKManager.data,
    qt_meta_data_SDKManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SDKManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SDKManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SDKManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SDKManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
