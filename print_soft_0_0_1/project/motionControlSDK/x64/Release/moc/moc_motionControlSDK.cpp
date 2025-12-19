/****************************************************************************
** Meta object code from reading C++ file 'motionControlSDK.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/sdk/motionControlSDK.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'motionControlSDK.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_motionControlSDK_t {
    QByteArrayData data[30];
    char stringdata0[345];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_motionControlSDK_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_motionControlSDK_t qt_meta_stringdata_motionControlSDK = {
    {
QT_MOC_LITERAL(0, 0, 16), // "motionControlSDK"
QT_MOC_LITERAL(1, 17, 9), // "connected"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "disconnected"
QT_MOC_LITERAL(4, 41, 16), // "connectedChanged"
QT_MOC_LITERAL(5, 58, 11), // "isConnected"
QT_MOC_LITERAL(6, 70, 15), // "deviceIpChanged"
QT_MOC_LITERAL(7, 86, 2), // "ip"
QT_MOC_LITERAL(8, 89, 17), // "devicePortChanged"
QT_MOC_LITERAL(9, 107, 4), // "port"
QT_MOC_LITERAL(10, 112, 13), // "errorOccurred"
QT_MOC_LITERAL(11, 126, 9), // "errorCode"
QT_MOC_LITERAL(12, 136, 12), // "errorMessage"
QT_MOC_LITERAL(13, 149, 11), // "infoMessage"
QT_MOC_LITERAL(14, 161, 7), // "message"
QT_MOC_LITERAL(15, 169, 10), // "logMessage"
QT_MOC_LITERAL(16, 180, 20), // "printProgressUpdated"
QT_MOC_LITERAL(17, 201, 8), // "progress"
QT_MOC_LITERAL(18, 210, 12), // "currentLayer"
QT_MOC_LITERAL(19, 223, 11), // "totalLayers"
QT_MOC_LITERAL(20, 235, 18), // "printStatusChanged"
QT_MOC_LITERAL(21, 254, 6), // "status"
QT_MOC_LITERAL(22, 261, 17), // "moveStatusChanged"
QT_MOC_LITERAL(23, 279, 15), // "positionUpdated"
QT_MOC_LITERAL(24, 295, 1), // "x"
QT_MOC_LITERAL(25, 297, 1), // "y"
QT_MOC_LITERAL(26, 299, 1), // "z"
QT_MOC_LITERAL(27, 301, 23), // "refreshConnectionStatus"
QT_MOC_LITERAL(28, 325, 8), // "deviceIp"
QT_MOC_LITERAL(29, 334, 10) // "devicePort"

    },
    "motionControlSDK\0connected\0\0disconnected\0"
    "connectedChanged\0isConnected\0"
    "deviceIpChanged\0ip\0devicePortChanged\0"
    "port\0errorOccurred\0errorCode\0errorMessage\0"
    "infoMessage\0message\0logMessage\0"
    "printProgressUpdated\0progress\0"
    "currentLayer\0totalLayers\0printStatusChanged\0"
    "status\0moveStatusChanged\0positionUpdated\0"
    "x\0y\0z\0refreshConnectionStatus\0deviceIp\0"
    "devicePort"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_motionControlSDK[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       3,  122, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    1,   81,    2, 0x06 /* Public */,
       6,    1,   84,    2, 0x06 /* Public */,
       8,    1,   87,    2, 0x06 /* Public */,
      10,    2,   90,    2, 0x06 /* Public */,
      13,    1,   95,    2, 0x06 /* Public */,
      15,    1,   98,    2, 0x06 /* Public */,
      16,    3,  101,    2, 0x06 /* Public */,
      20,    1,  108,    2, 0x06 /* Public */,
      22,    1,  111,    2, 0x06 /* Public */,
      23,    3,  114,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      27,    0,  121,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::UShort,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   11,   12,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   17,   18,   19,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double,   24,   25,   26,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       1, QMetaType::Bool, 0x00495001,
      28, QMetaType::QString, 0x00495001,
      29, QMetaType::UShort, 0x00495001,

 // properties: notify_signal_id
       2,
       3,
       4,

       0        // eod
};

void motionControlSDK::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<motionControlSDK *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->deviceIpChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->devicePortChanged((*reinterpret_cast< quint16(*)>(_a[1]))); break;
        case 5: _t->errorOccurred((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->infoMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->printProgressUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 9: _t->printStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->moveStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->positionUpdated((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 12: _t->refreshConnectionStatus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (motionControlSDK::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::connectedChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::deviceIpChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(quint16 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::devicePortChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::errorOccurred)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::infoMessage)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::logMessage)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::printProgressUpdated)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::printStatusChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::moveStatusChanged)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (motionControlSDK::*)(double , double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&motionControlSDK::positionUpdated)) {
                *result = 11;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<motionControlSDK *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->deviceIp(); break;
        case 2: *reinterpret_cast< quint16*>(_v) = _t->devicePort(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject motionControlSDK::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_motionControlSDK.data,
    qt_meta_data_motionControlSDK,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *motionControlSDK::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *motionControlSDK::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_motionControlSDK.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int motionControlSDK::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void motionControlSDK::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void motionControlSDK::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void motionControlSDK::connectedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void motionControlSDK::deviceIpChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void motionControlSDK::devicePortChanged(quint16 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void motionControlSDK::errorOccurred(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void motionControlSDK::infoMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void motionControlSDK::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void motionControlSDK::printProgressUpdated(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void motionControlSDK::printStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void motionControlSDK::moveStatusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void motionControlSDK::positionUpdated(double _t1, double _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
