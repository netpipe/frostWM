/****************************************************************************
** Meta object code from reading C++ file 'joypad.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "joypad.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'joypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_JoyPad[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,    8,    7,    7, 0x0a,
      32,    8,    7,    7, 0x0a,
      62,   47,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_JoyPad[] = {
    "JoyPad\0\0fd\0handleJoyEvents(int)\0"
    "errorRead(int)\0windowHasFocus\0"
    "focusChange(bool)\0"
};

void JoyPad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        JoyPad *_t = static_cast<JoyPad *>(_o);
        switch (_id) {
        case 0: _t->handleJoyEvents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->errorRead((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->focusChange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData JoyPad::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject JoyPad::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_JoyPad,
      qt_meta_data_JoyPad, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &JoyPad::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *JoyPad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *JoyPad::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_JoyPad))
        return static_cast<void*>(const_cast< JoyPad*>(this));
    return QObject::qt_metacast(_clname);
}

int JoyPad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
