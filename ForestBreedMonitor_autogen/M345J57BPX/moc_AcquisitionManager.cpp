/****************************************************************************
** Meta object code from reading C++ file 'AcquisitionManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/business/AcquisitionManager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AcquisitionManager.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_sensor__AcquisitionManager_t {
    uint offsetsAndSizes[38];
    char stringdata0[27];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[6];
    char stringdata5[14];
    char stringdata6[19];
    char stringdata7[6];
    char stringdata8[20];
    char stringdata9[21];
    char stringdata10[7];
    char stringdata11[13];
    char stringdata12[9];
    char stringdata13[12];
    char stringdata14[14];
    char stringdata15[15];
    char stringdata16[17];
    char stringdata17[8];
    char stringdata18[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_sensor__AcquisitionManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_sensor__AcquisitionManager_t qt_meta_stringdata_sensor__AcquisitionManager = {
    {
        QT_MOC_LITERAL(0, 26),  // "sensor::AcquisitionManager"
        QT_MOC_LITERAL(27, 9),  // "dataReady"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 22),  // "sensor::SensorFramePtr"
        QT_MOC_LITERAL(61, 5),  // "frame"
        QT_MOC_LITERAL(67, 13),  // "alarmOccurred"
        QT_MOC_LITERAL(81, 18),  // "sensor::AlarmEvent"
        QT_MOC_LITERAL(100, 5),  // "alarm"
        QT_MOC_LITERAL(106, 19),  // "deviceStatusChanged"
        QT_MOC_LITERAL(126, 20),  // "sensor::DeviceStatus"
        QT_MOC_LITERAL(147, 6),  // "status"
        QT_MOC_LITERAL(154, 12),  // "statsUpdated"
        QT_MOC_LITERAL(167, 8),  // "uint64_t"
        QT_MOC_LITERAL(176, 11),  // "totalFrames"
        QT_MOC_LITERAL(188, 13),  // "droppedFrames"
        QT_MOC_LITERAL(202, 14),  // "onNetworkFrame"
        QT_MOC_LITERAL(217, 16),  // "onProcessedFrame"
        QT_MOC_LITERAL(234, 7),  // "onAlarm"
        QT_MOC_LITERAL(242, 15)   // "onStatusChanged"
    },
    "sensor::AcquisitionManager",
    "dataReady",
    "",
    "sensor::SensorFramePtr",
    "frame",
    "alarmOccurred",
    "sensor::AlarmEvent",
    "alarm",
    "deviceStatusChanged",
    "sensor::DeviceStatus",
    "status",
    "statsUpdated",
    "uint64_t",
    "totalFrames",
    "droppedFrames",
    "onNetworkFrame",
    "onProcessedFrame",
    "onAlarm",
    "onStatusChanged"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_sensor__AcquisitionManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    1 /* Public */,
       5,    1,   65,    2, 0x06,    3 /* Public */,
       8,    1,   68,    2, 0x06,    5 /* Public */,
      11,    2,   71,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    1,   76,    2, 0x08,   10 /* Private */,
      16,    1,   79,    2, 0x08,   12 /* Private */,
      17,    1,   82,    2, 0x08,   14 /* Private */,
      18,    1,   85,    2, 0x08,   16 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,   13,   14,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject sensor::AcquisitionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_sensor__AcquisitionManager.offsetsAndSizes,
    qt_meta_data_sensor__AcquisitionManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_sensor__AcquisitionManager_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AcquisitionManager, std::true_type>,
        // method 'dataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::SensorFramePtr, std::false_type>,
        // method 'alarmOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::AlarmEvent, std::false_type>,
        // method 'deviceStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::DeviceStatus, std::false_type>,
        // method 'statsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint64_t, std::false_type>,
        // method 'onNetworkFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::SensorFramePtr, std::false_type>,
        // method 'onProcessedFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::SensorFramePtr, std::false_type>,
        // method 'onAlarm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::AlarmEvent, std::false_type>,
        // method 'onStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::DeviceStatus, std::false_type>
    >,
    nullptr
} };

void sensor::AcquisitionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AcquisitionManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataReady((*reinterpret_cast< std::add_pointer_t<sensor::SensorFramePtr>>(_a[1]))); break;
        case 1: _t->alarmOccurred((*reinterpret_cast< std::add_pointer_t<sensor::AlarmEvent>>(_a[1]))); break;
        case 2: _t->deviceStatusChanged((*reinterpret_cast< std::add_pointer_t<sensor::DeviceStatus>>(_a[1]))); break;
        case 3: _t->statsUpdated((*reinterpret_cast< std::add_pointer_t<uint64_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint64_t>>(_a[2]))); break;
        case 4: _t->onNetworkFrame((*reinterpret_cast< std::add_pointer_t<sensor::SensorFramePtr>>(_a[1]))); break;
        case 5: _t->onProcessedFrame((*reinterpret_cast< std::add_pointer_t<sensor::SensorFramePtr>>(_a[1]))); break;
        case 6: _t->onAlarm((*reinterpret_cast< std::add_pointer_t<sensor::AlarmEvent>>(_a[1]))); break;
        case 7: _t->onStatusChanged((*reinterpret_cast< std::add_pointer_t<sensor::DeviceStatus>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::SensorFramePtr >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::AlarmEvent >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::DeviceStatus >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::SensorFramePtr >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::SensorFramePtr >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::AlarmEvent >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::DeviceStatus >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AcquisitionManager::*)(sensor::SensorFramePtr );
            if (_t _q_method = &AcquisitionManager::dataReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AcquisitionManager::*)(sensor::AlarmEvent );
            if (_t _q_method = &AcquisitionManager::alarmOccurred; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AcquisitionManager::*)(sensor::DeviceStatus );
            if (_t _q_method = &AcquisitionManager::deviceStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AcquisitionManager::*)(uint64_t , uint64_t );
            if (_t _q_method = &AcquisitionManager::statsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *sensor::AcquisitionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sensor::AcquisitionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sensor__AcquisitionManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int sensor::AcquisitionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void sensor::AcquisitionManager::dataReady(sensor::SensorFramePtr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sensor::AcquisitionManager::alarmOccurred(sensor::AlarmEvent _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void sensor::AcquisitionManager::deviceStatusChanged(sensor::DeviceStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void sensor::AcquisitionManager::statsUpdated(uint64_t _t1, uint64_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
