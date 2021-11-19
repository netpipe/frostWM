/****************************************************************************
** Meta object code from reading C++ file 'layout.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/layout.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LayoutManager_t {
    QByteArrayData data[19];
    char stringdata0[187];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LayoutManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LayoutManager_t qt_meta_stringdata_LayoutManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "LayoutManager"
QT_MOC_LITERAL(1, 14, 4), // "load"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "name"
QT_MOC_LITERAL(4, 25, 6), // "reload"
QT_MOC_LITERAL(5, 32, 5), // "clear"
QT_MOC_LITERAL(6, 38, 4), // "save"
QT_MOC_LITERAL(7, 43, 6), // "saveAs"
QT_MOC_LITERAL(8, 50, 11), // "saveDefault"
QT_MOC_LITERAL(9, 62, 6), // "remove"
QT_MOC_LITERAL(10, 69, 9), // "iconClick"
QT_MOC_LITERAL(11, 79, 9), // "trayClick"
QT_MOC_LITERAL(12, 89, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(13, 123, 6), // "reason"
QT_MOC_LITERAL(14, 130, 8), // "trayMenu"
QT_MOC_LITERAL(15, 139, 8), // "QAction*"
QT_MOC_LITERAL(16, 148, 14), // "menuItemAction"
QT_MOC_LITERAL(17, 163, 9), // "fillPopup"
QT_MOC_LITERAL(18, 173, 13) // "updateJoyDevs"

    },
    "LayoutManager\0load\0\0name\0reload\0clear\0"
    "save\0saveAs\0saveDefault\0remove\0iconClick\0"
    "trayClick\0QSystemTrayIcon::ActivationReason\0"
    "reason\0trayMenu\0QAction*\0menuItemAction\0"
    "fillPopup\0updateJoyDevs"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LayoutManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x0a /* Public */,
       1,    0,   82,    2, 0x0a /* Public */,
       4,    0,   83,    2, 0x0a /* Public */,
       5,    0,   84,    2, 0x0a /* Public */,
       6,    0,   85,    2, 0x0a /* Public */,
       7,    0,   86,    2, 0x0a /* Public */,
       8,    0,   87,    2, 0x0a /* Public */,
       9,    0,   88,    2, 0x0a /* Public */,
      10,    0,   89,    2, 0x0a /* Public */,
      11,    1,   90,    2, 0x0a /* Public */,
      14,    1,   93,    2, 0x0a /* Public */,
      17,    0,   96,    2, 0x0a /* Public */,
      18,    0,   97,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    3,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LayoutManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LayoutManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->load((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->load();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->reload();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LayoutManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_LayoutManager.data,
    qt_meta_data_LayoutManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LayoutManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LayoutManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LayoutManager.stringdata0))
        return static_cast<void*>(this);
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
