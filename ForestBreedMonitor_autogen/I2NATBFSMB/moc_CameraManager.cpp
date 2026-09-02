/****************************************************************************
** Meta object code from reading C++ file 'CameraManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/camera/CameraManager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CameraManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_sensor__CameraManager_t {
    uint offsetsAndSizes[26];
    char stringdata0[22];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[10];
    char stringdata5[20];
    char stringdata6[10];
    char stringdata7[14];
    char stringdata8[7];
    char stringdata9[19];
    char stringdata10[12];
    char stringdata11[12];
    char stringdata12[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_sensor__CameraManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_sensor__CameraManager_t qt_meta_stringdata_sensor__CameraManager = {
    {
        QT_MOC_LITERAL(0, 21),  // "sensor::CameraManager"
        QT_MOC_LITERAL(22, 13),  // "photoCaptured"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 8),  // "filePath"
        QT_MOC_LITERAL(46, 9),  // "timestamp"
        QT_MOC_LITERAL(56, 19),  // "cameraStatusChanged"
        QT_MOC_LITERAL(76, 9),  // "available"
        QT_MOC_LITERAL(86, 13),  // "captureFailed"
        QT_MOC_LITERAL(100, 6),  // "reason"
        QT_MOC_LITERAL(107, 18),  // "cameraStatsUpdated"
        QT_MOC_LITERAL(126, 11),  // "totalPhotos"
        QT_MOC_LITERAL(138, 11),  // "lastCapture"
        QT_MOC_LITERAL(150, 14)   // "onTimerTimeout"
    },
    "sensor::CameraManager",
    "photoCaptured",
    "",
    "filePath",
    "timestamp",
    "cameraStatusChanged",
    "available",
    "captureFailed",
    "reason",
    "cameraStatsUpdated",
    "totalPhotos",
    "lastCapture",
    "onTimerTimeout"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_sensor__CameraManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   44,    2, 0x06,    1 /* Public */,
       5,    1,   49,    2, 0x06,    4 /* Public */,
       7,    1,   52,    2, 0x06,    6 /* Public */,
       9,    2,   55,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   60,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   10,   11,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject sensor::CameraManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_sensor__CameraManager.offsetsAndSizes,
    qt_meta_data_sensor__CameraManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_sensor__CameraManager_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CameraManager, std::true_type>,
        // method 'photoCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cameraStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'captureFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'cameraStatsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTimerTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void sensor::CameraManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CameraManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->photoCaptured((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->cameraStatusChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->captureFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->cameraStatsUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->onTimerTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CameraManager::*)(const QString & , const QString & );
            if (_t _q_method = &CameraManager::photoCaptured; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(bool );
            if (_t _q_method = &CameraManager::cameraStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(const QString & );
            if (_t _q_method = &CameraManager::captureFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(int , const QString & );
            if (_t _q_method = &CameraManager::cameraStatsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *sensor::CameraManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sensor::CameraManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sensor__CameraManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int sensor::CameraManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void sensor::CameraManager::photoCaptured(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sensor::CameraManager::cameraStatusChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void sensor::CameraManager::captureFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void sensor::CameraManager::cameraStatsUpdated(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
