/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/MainWindow.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_sensor__MainWindow_t {
    uint offsetsAndSizes[36];
    char stringdata0[19];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[12];
    char stringdata5[23];
    char stringdata6[6];
    char stringdata7[8];
    char stringdata8[19];
    char stringdata9[6];
    char stringdata10[16];
    char stringdata11[21];
    char stringdata12[7];
    char stringdata13[15];
    char stringdata14[9];
    char stringdata15[6];
    char stringdata16[8];
    char stringdata17[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_sensor__MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_sensor__MainWindow_t qt_meta_stringdata_sensor__MainWindow = {
    {
        QT_MOC_LITERAL(0, 18),  // "sensor::MainWindow"
        QT_MOC_LITERAL(19, 14),  // "onStartClicked"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 13),  // "onStopClicked"
        QT_MOC_LITERAL(49, 11),  // "onDataReady"
        QT_MOC_LITERAL(61, 22),  // "sensor::SensorFramePtr"
        QT_MOC_LITERAL(84, 5),  // "frame"
        QT_MOC_LITERAL(90, 7),  // "onAlarm"
        QT_MOC_LITERAL(98, 18),  // "sensor::AlarmEvent"
        QT_MOC_LITERAL(117, 5),  // "alarm"
        QT_MOC_LITERAL(123, 15),  // "onStatusChanged"
        QT_MOC_LITERAL(139, 20),  // "sensor::DeviceStatus"
        QT_MOC_LITERAL(160, 6),  // "status"
        QT_MOC_LITERAL(167, 14),  // "onStatsUpdated"
        QT_MOC_LITERAL(182, 8),  // "uint64_t"
        QT_MOC_LITERAL(191, 5),  // "total"
        QT_MOC_LITERAL(197, 7),  // "dropped"
        QT_MOC_LITERAL(205, 11)   // "onUiRefresh"
    },
    "sensor::MainWindow",
    "onStartClicked",
    "",
    "onStopClicked",
    "onDataReady",
    "sensor::SensorFramePtr",
    "frame",
    "onAlarm",
    "sensor::AlarmEvent",
    "alarm",
    "onStatusChanged",
    "sensor::DeviceStatus",
    "status",
    "onStatsUpdated",
    "uint64_t",
    "total",
    "dropped",
    "onUiRefresh"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_sensor__MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    1,   58,    2, 0x08,    3 /* Private */,
       7,    1,   61,    2, 0x08,    5 /* Private */,
      10,    1,   64,    2, 0x08,    7 /* Private */,
      13,    2,   67,    2, 0x08,    9 /* Private */,
      17,    0,   72,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,   15,   16,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject sensor::MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_sensor__MainWindow.offsetsAndSizes,
    qt_meta_data_sensor__MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_sensor__MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onStartClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStopClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::SensorFramePtr, std::false_type>,
        // method 'onAlarm'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::AlarmEvent, std::false_type>,
        // method 'onStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<sensor::DeviceStatus, std::false_type>,
        // method 'onStatsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint64_t, std::false_type>,
        // method 'onUiRefresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void sensor::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onStartClicked(); break;
        case 1: _t->onStopClicked(); break;
        case 2: _t->onDataReady((*reinterpret_cast< std::add_pointer_t<sensor::SensorFramePtr>>(_a[1]))); break;
        case 3: _t->onAlarm((*reinterpret_cast< std::add_pointer_t<sensor::AlarmEvent>>(_a[1]))); break;
        case 4: _t->onStatusChanged((*reinterpret_cast< std::add_pointer_t<sensor::DeviceStatus>>(_a[1]))); break;
        case 5: _t->onStatsUpdated((*reinterpret_cast< std::add_pointer_t<uint64_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint64_t>>(_a[2]))); break;
        case 6: _t->onUiRefresh(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::SensorFramePtr >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::AlarmEvent >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< sensor::DeviceStatus >(); break;
            }
            break;
        }
    }
}

const QMetaObject *sensor::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sensor::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sensor__MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int sensor::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
