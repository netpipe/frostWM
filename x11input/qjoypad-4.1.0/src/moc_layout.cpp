/****************************************************************************
** Meta object code from reading C++ file 'layout.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "layout.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LayoutManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   20,   15,   14, 0x0a,
      39,   14,   15,   14, 0x0a,
      46,   14,   15,   14, 0x0a,
      55,   14,   14,   14, 0x0a,
      63,   14,   14,   14, 0x0a,
      70,   14,   14,   14, 0x0a,
      79,   14,   14,   14, 0x0a,
      93,   14,   14,   14, 0x0a,
     102,   14,   14,   14, 0x0a,
     121,  114,   14,   14, 0x0a,
     181,  166,   14,   14, 0x0a,
     200,   14,   14,   14, 0x0a,
     212,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LayoutManager[] = {
    "LayoutManager\0\0bool\0name\0load(QString)\0"
    "load()\0reload()\0clear()\0save()\0saveAs()\0"
    "saveDefault()\0remove()\0iconClick()\0"
    "reason\0trayClick(QSystemTrayIcon::ActivationReason)\0"
    "menuItemAction\0trayMenu(QAction*)\0"
    "fillPopup()\0updateJoyDevs()\0"
};

void LayoutManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LayoutManager *_t = static_cast<LayoutManager *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->load((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = _t->load();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->reload();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->clear(); break;
        case 4: _t->save(); break;
        case 5: _t->saveAs(); break;
        case 6: _t->saveDefault(); break;
        case 7: _t->remove(); break;
        case 8: _t->iconClick(); break;
        case 9: _t->trayClick((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 10: _t->trayMenu((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 11: _t->fillPopup(); break;
        case 12: _t->updateJoyDevs(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LayoutManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LayoutManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LayoutManager,
      qt_meta_data_LayoutManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LayoutManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LayoutManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LayoutManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LayoutManager))
        return static_cast<void*>(const_cast< LayoutManager*>(this));
    return QObject::qt_metacast(_clname);
}

int LayoutManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
