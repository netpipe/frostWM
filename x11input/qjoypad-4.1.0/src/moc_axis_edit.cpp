/****************************************************************************
** Meta object code from reading C++ file 'axis_edit.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "axis_edit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'axis_edit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AxisEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   10,    9,    9, 0x09,
      42,   36,    9,    9, 0x09,
      60,   36,    9,    9, 0x09,
      87,   36,    9,    9, 0x09,
     109,    9,    9,    9, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_AxisEdit[] = {
    "AxisEdit\0\0on\0CGradientChanged(bool)\0"
    "index\0CModeChanged(int)\0"
    "CTransferCurveChanged(int)\0"
    "CThrottleChanged(int)\0accept()\0"
};

void AxisEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AxisEdit *_t = static_cast<AxisEdit *>(_o);
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

const QMetaObjectExtraData AxisEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AxisEdit::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AxisEdit,
      qt_meta_data_AxisEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AxisEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AxisEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AxisEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AxisEdit))
        return static_cast<void*>(const_cast< AxisEdit*>(this));
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
