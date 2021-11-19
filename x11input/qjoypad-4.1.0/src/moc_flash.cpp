/****************************************************************************
** Meta object code from reading C++ file 'flash.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "flash.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'flash.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FlashButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FlashButton[] = {
    "FlashButton\0\0on\0flashed(bool)\0flash()\0"
};

void FlashButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FlashButton *_t = static_cast<FlashButton *>(_o);
        switch (_id) {
        case 0: _t->flashed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->flash(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FlashButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FlashButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_FlashButton,
      qt_meta_data_FlashButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FlashButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FlashButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FlashButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FlashButton))
        return static_cast<void*>(const_cast< FlashButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int FlashButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void FlashButton::flashed(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_FlashRadioArray[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   16,   16,   16, 0x0a,
      41,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FlashRadioArray[] = {
    "FlashRadioArray\0\0changed(int)\0flash(int)\0"
    "clicked()\0"
};

void FlashRadioArray::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FlashRadioArray *_t = static_cast<FlashRadioArray *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->flash((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FlashRadioArray::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FlashRadioArray::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FlashRadioArray,
      qt_meta_data_FlashRadioArray, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FlashRadioArray::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FlashRadioArray::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FlashRadioArray::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FlashRadioArray))
        return static_cast<void*>(const_cast< FlashRadioArray*>(this));
    return QWidget::qt_metacast(_clname);
}

int FlashRadioArray::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FlashRadioArray::changed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
