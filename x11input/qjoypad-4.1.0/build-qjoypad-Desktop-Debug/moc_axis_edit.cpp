/****************************************************************************
** Meta object code from reading C++ file 'axis_edit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/axis_edit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'axis_edit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AxisEdit_t {
    QByteArrayData data[9];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AxisEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AxisEdit_t qt_meta_stringdata_AxisEdit = {
    {
QT_MOC_LITERAL(0, 0, 8), // "AxisEdit"
QT_MOC_LITERAL(1, 9, 16), // "CGradientChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 2), // "on"
QT_MOC_LITERAL(4, 30, 12), // "CModeChanged"
QT_MOC_LITERAL(5, 43, 5), // "index"
QT_MOC_LITERAL(6, 49, 21), // "CTransferCurveChanged"
QT_MOC_LITERAL(7, 71, 16), // "CThrottleChanged"
QT_MOC_LITERAL(8, 88, 6) // "accept"

    },
    "AxisEdit\0CGradientChanged\0\0on\0"
    "CModeChanged\0index\0CTransferCurveChanged\0"
    "CThrottleChanged\0accept"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AxisEdit[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x09 /* Protected */,
       4,    1,   42,    2, 0x09 /* Protected */,
       6,    1,   45,    2, 0x09 /* Protected */,
       7,    1,   48,    2, 0x09 /* Protected */,
       8,    0,   51,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,

       0        // eod
};

void AxisEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AxisEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->CGradientChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->CModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->CTransferCurveChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->CThrottleChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->accept(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AxisEdit::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_AxisEdit.data,
    qt_meta_data_AxisEdit,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AxisEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AxisEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AxisEdit.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AxisEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
