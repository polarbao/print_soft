/****************************************************************************
** Meta object code from reading C++ file 'TcpClient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/communicate/TcpClient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TcpClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TcpClient_t {
    QByteArrayData data[10];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TcpClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TcpClient_t qt_meta_stringdata_TcpClient = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TcpClient"
QT_MOC_LITERAL(1, 10, 10), // "sigNewData"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 3), // "msg"
QT_MOC_LITERAL(4, 26, 8), // "sigError"
QT_MOC_LITERAL(5, 35, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 64, 11), // "socketError"
QT_MOC_LITERAL(7, 76, 21), // "sigSocketStateChanged"
QT_MOC_LITERAL(8, 98, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(9, 127, 5) // "state"

    },
    "TcpClient\0sigNewData\0\0msg\0sigError\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "sigSocketStateChanged\0"
    "QAbstractSocket::SocketState\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TcpClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       7,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void TcpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigNewData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->sigError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 2: _t->sigSocketStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TcpClient::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClient::sigNewData)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TcpClient::*)(QAbstractSocket::SocketError );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClient::sigError)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TcpClient::*)(QAbstractSocket::SocketState );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClient::sigSocketStateChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TcpClient::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TcpClient.data,
    qt_meta_data_TcpClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TcpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TcpClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TcpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TcpClient::sigNewData(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TcpClient::sigError(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TcpClient::sigSocketStateChanged(QAbstractSocket::SocketState _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_TcpClientImpl_t {
    QByteArrayData data[17];
    char stringdata0[209];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TcpClientImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TcpClientImpl_t qt_meta_stringdata_TcpClientImpl = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TcpClientImpl"
QT_MOC_LITERAL(1, 14, 10), // "sigNewData"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 3), // "msg"
QT_MOC_LITERAL(4, 30, 8), // "sigError"
QT_MOC_LITERAL(5, 39, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 68, 11), // "socketError"
QT_MOC_LITERAL(7, 80, 14), // "sigSocketState"
QT_MOC_LITERAL(8, 95, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(9, 124, 5), // "state"
QT_MOC_LITERAL(10, 130, 11), // "isConnected"
QT_MOC_LITERAL(11, 142, 9), // "onConnect"
QT_MOC_LITERAL(12, 152, 12), // "onDisconnect"
QT_MOC_LITERAL(13, 165, 10), // "onReadData"
QT_MOC_LITERAL(14, 176, 9), // "onTimeout"
QT_MOC_LITERAL(15, 186, 7), // "onError"
QT_MOC_LITERAL(16, 194, 14) // "onStateChanged"

    },
    "TcpClientImpl\0sigNewData\0\0msg\0sigError\0"
    "QAbstractSocket::SocketError\0socketError\0"
    "sigSocketState\0QAbstractSocket::SocketState\0"
    "state\0isConnected\0onConnect\0onDisconnect\0"
    "onReadData\0onTimeout\0onError\0"
    "onStateChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TcpClientImpl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       7,    1,   70,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   73,    2, 0x0a /* Public */,
      11,    0,   74,    2, 0x0a /* Public */,
      12,    0,   75,    2, 0x0a /* Public */,
      13,    0,   76,    2, 0x0a /* Public */,
      14,    0,   77,    2, 0x0a /* Public */,
      15,    1,   78,    2, 0x0a /* Public */,
      16,    1,   81,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void TcpClientImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpClientImpl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigNewData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: _t->sigError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 2: _t->sigSocketState((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 3: { bool _r = _t->isConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->onConnect(); break;
        case 5: _t->onDisconnect(); break;
        case 6: _t->onReadData(); break;
        case 7: _t->onTimeout(); break;
        case 8: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->onStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
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
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TcpClientImpl::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClientImpl::sigNewData)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TcpClientImpl::*)(QAbstractSocket::SocketError );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClientImpl::sigError)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TcpClientImpl::*)(QAbstractSocket::SocketState );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TcpClientImpl::sigSocketState)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TcpClientImpl::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TcpClientImpl.data,
    qt_meta_data_TcpClientImpl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TcpClientImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpClientImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TcpClientImpl.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TcpClientImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void TcpClientImpl::sigNewData(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TcpClientImpl::sigError(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TcpClientImpl::sigSocketState(QAbstractSocket::SocketState _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
