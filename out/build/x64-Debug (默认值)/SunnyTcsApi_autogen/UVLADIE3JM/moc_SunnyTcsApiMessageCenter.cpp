/****************************************************************************
** Meta object code from reading C++ file 'SunnyTcsApiMessageCenter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/SunnyTcsApiMessageCenter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SunnyTcsApiMessageCenter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SunnyTcsApiMessageCenter_t {
    QByteArrayData data[7];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SunnyTcsApiMessageCenter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SunnyTcsApiMessageCenter_t qt_meta_stringdata_SunnyTcsApiMessageCenter = {
    {
QT_MOC_LITERAL(0, 0, 24), // "SunnyTcsApiMessageCenter"
QT_MOC_LITERAL(1, 25, 12), // "disconnected"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "modelSyncDataReady"
QT_MOC_LITERAL(4, 58, 5), // "bytes"
QT_MOC_LITERAL(5, 64, 20), // "agvFeedbackDataReady"
QT_MOC_LITERAL(6, 85, 21) // "lineFeedbackDataReady"

    },
    "SunnyTcsApiMessageCenter\0disconnected\0"
    "\0modelSyncDataReady\0bytes\0"
    "agvFeedbackDataReady\0lineFeedbackDataReady"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SunnyTcsApiMessageCenter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,
       5,    1,   38,    2, 0x06 /* Public */,
       6,    1,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    4,
    QMetaType::Void, QMetaType::QByteArrayList,    4,
    QMetaType::Void, QMetaType::QByteArray,    4,

       0        // eod
};

void SunnyTcsApiMessageCenter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SunnyTcsApiMessageCenter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->disconnected(); break;
        case 1: _t->modelSyncDataReady((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 2: _t->agvFeedbackDataReady((*reinterpret_cast< const QList<QByteArray>(*)>(_a[1]))); break;
        case 3: _t->lineFeedbackDataReady((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SunnyTcsApiMessageCenter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SunnyTcsApiMessageCenter::disconnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SunnyTcsApiMessageCenter::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SunnyTcsApiMessageCenter::modelSyncDataReady)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SunnyTcsApiMessageCenter::*)(const QList<QByteArray> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SunnyTcsApiMessageCenter::agvFeedbackDataReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SunnyTcsApiMessageCenter::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SunnyTcsApiMessageCenter::lineFeedbackDataReady)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SunnyTcsApiMessageCenter::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_SunnyTcsApiMessageCenter.data,
    qt_meta_data_SunnyTcsApiMessageCenter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SunnyTcsApiMessageCenter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SunnyTcsApiMessageCenter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SunnyTcsApiMessageCenter.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int SunnyTcsApiMessageCenter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SunnyTcsApiMessageCenter::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SunnyTcsApiMessageCenter::modelSyncDataReady(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SunnyTcsApiMessageCenter::agvFeedbackDataReady(const QList<QByteArray> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SunnyTcsApiMessageCenter::lineFeedbackDataReady(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
