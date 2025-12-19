/****************************************************************************
** Meta object code from reading C++ file 'ProtocolPrint.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/protocol/ProtocolPrint.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProtocolPrint.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProtocolPrint_t {
    QByteArrayData data[32];
    char stringdata0[328];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProtocolPrint_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProtocolPrint_t qt_meta_stringdata_ProtocolPrint = {
    {
QT_MOC_LITERAL(0, 0, 13), // "ProtocolPrint"
QT_MOC_LITERAL(1, 14, 12), // "SigHeartBeat"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "SigCmdReply"
QT_MOC_LITERAL(4, 40, 3), // "cmd"
QT_MOC_LITERAL(5, 44, 7), // "errCode"
QT_MOC_LITERAL(6, 52, 3), // "arr"
QT_MOC_LITERAL(7, 56, 11), // "sigDataInfo"
QT_MOC_LITERAL(8, 68, 14), // "DataFieldInfo1"
QT_MOC_LITERAL(9, 83, 18), // "sigSpeedRotatePara"
QT_MOC_LITERAL(10, 102, 6), // "Decode"
QT_MOC_LITERAL(11, 109, 8), // "datagram"
QT_MOC_LITERAL(12, 118, 15), // "GetSendDatagram"
QT_MOC_LITERAL(13, 134, 7), // "FunCode"
QT_MOC_LITERAL(14, 142, 4), // "code"
QT_MOC_LITERAL(15, 147, 4), // "data"
QT_MOC_LITERAL(16, 152, 18), // "GetSendImgDatagram"
QT_MOC_LITERAL(17, 171, 1), // "w"
QT_MOC_LITERAL(18, 173, 1), // "h"
QT_MOC_LITERAL(19, 175, 7), // "Imgtype"
QT_MOC_LITERAL(20, 183, 7), // "hexData"
QT_MOC_LITERAL(21, 191, 19), // "GetResponseDatagram"
QT_MOC_LITERAL(22, 211, 9), // "SplitComm"
QT_MOC_LITERAL(23, 221, 8), // "uint16_t"
QT_MOC_LITERAL(24, 230, 7), // "command"
QT_MOC_LITERAL(25, 238, 8), // "uint8_t&"
QT_MOC_LITERAL(26, 247, 8), // "highByte"
QT_MOC_LITERAL(27, 256, 7), // "lowByte"
QT_MOC_LITERAL(28, 264, 18), // "GetSerResponseComm"
QT_MOC_LITERAL(29, 283, 9), // "clientCmd"
QT_MOC_LITERAL(30, 293, 20), // "GetClientCommFromSer"
QT_MOC_LITERAL(31, 314, 13) // "serverCommand"

    },
    "ProtocolPrint\0SigHeartBeat\0\0SigCmdReply\0"
    "cmd\0errCode\0arr\0sigDataInfo\0DataFieldInfo1\0"
    "sigSpeedRotatePara\0Decode\0datagram\0"
    "GetSendDatagram\0FunCode\0code\0data\0"
    "GetSendImgDatagram\0w\0h\0Imgtype\0hexData\0"
    "GetResponseDatagram\0SplitComm\0uint16_t\0"
    "command\0uint8_t&\0highByte\0lowByte\0"
    "GetSerResponseComm\0clientCmd\0"
    "GetClientCommFromSer\0serverCommand"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProtocolPrint[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    3,   85,    2, 0x06 /* Public */,
       3,    2,   92,    2, 0x26 /* Public | MethodCloned */,
       7,    1,   97,    2, 0x06 /* Public */,
       9,    0,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  101,    2, 0x0a /* Public */,
      12,    2,  104,    2, 0x0a /* Public */,
      12,    1,  109,    2, 0x2a /* Public | MethodCloned */,
      16,    4,  112,    2, 0x0a /* Public */,
      21,    2,  121,    2, 0x0a /* Public */,
      21,    1,  126,    2, 0x2a /* Public | MethodCloned */,
      22,    3,  129,    2, 0x0a /* Public */,
      28,    1,  136,    2, 0x0a /* Public */,
      30,    1,  139,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::UChar, QMetaType::QByteArray,    4,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::UChar,    4,    5,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,   11,
    QMetaType::QByteArray, 0x80000000 | 13, QMetaType::QByteArray,   14,   15,
    QMetaType::QByteArray, 0x80000000 | 13,   14,
    QMetaType::QByteArrayList, QMetaType::UShort, QMetaType::UShort, QMetaType::UChar, QMetaType::QByteArray,   17,   18,   19,   20,
    QMetaType::QByteArray, 0x80000000 | 13, QMetaType::QByteArray,   14,   15,
    QMetaType::QByteArray, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 23, 0x80000000 | 25, 0x80000000 | 25,   24,   26,   27,
    0x80000000 | 23, 0x80000000 | 13,   29,
    0x80000000 | 13, 0x80000000 | 23,   31,

       0        // eod
};

void ProtocolPrint::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProtocolPrint *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SigHeartBeat(); break;
        case 1: _t->SigCmdReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uchar(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3]))); break;
        case 2: _t->SigCmdReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uchar(*)>(_a[2]))); break;
        case 3: _t->sigDataInfo((*reinterpret_cast< DataFieldInfo1(*)>(_a[1]))); break;
        case 4: _t->sigSpeedRotatePara(); break;
        case 5: _t->Decode((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 6: { QByteArray _r = _t->GetSendDatagram((*reinterpret_cast< FunCode(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 7: { QByteArray _r = _t->GetSendDatagram((*reinterpret_cast< FunCode(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 8: { QList<QByteArray> _r = _t->GetSendImgDatagram((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint8(*)>(_a[3])),(*reinterpret_cast< const QByteArray(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QList<QByteArray>*>(_a[0]) = std::move(_r); }  break;
        case 9: { QByteArray _r = _t->GetResponseDatagram((*reinterpret_cast< FunCode(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 10: { QByteArray _r = _t->GetResponseDatagram((*reinterpret_cast< FunCode(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QByteArray*>(_a[0]) = std::move(_r); }  break;
        case 11: _t->SplitComm((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2])),(*reinterpret_cast< uint8_t(*)>(_a[3]))); break;
        case 12: { uint16_t _r = _t->GetSerResponseComm((*reinterpret_cast< FunCode(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint16_t*>(_a[0]) = std::move(_r); }  break;
        case 13: { FunCode _r = _t->GetClientCommFromSer((*reinterpret_cast< uint16_t(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< FunCode*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProtocolPrint::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolPrint::SigHeartBeat)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProtocolPrint::*)(int , uchar , QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolPrint::SigCmdReply)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProtocolPrint::*)(DataFieldInfo1 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolPrint::sigDataInfo)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProtocolPrint::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProtocolPrint::sigSpeedRotatePara)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProtocolPrint::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_ProtocolPrint.data,
    qt_meta_data_ProtocolPrint,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProtocolPrint::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProtocolPrint::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProtocolPrint.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProtocolPrint::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ProtocolPrint::SigHeartBeat()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ProtocolPrint::SigCmdReply(int _t1, uchar _t2, QByteArray _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 3
void ProtocolPrint::sigDataInfo(DataFieldInfo1 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ProtocolPrint::sigSpeedRotatePara()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
