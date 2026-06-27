#include <QtQml/qqmlprivate.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

namespace QmlCacheGeneratedCode {
namespace _qt_qml_demos_calqlatr_Main_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_ApplicationState_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_Display_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_NumberPad_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_CalculatorButton_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_BackspaceButton_qml { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}
namespace _qt_qml_demos_calqlatr_content_calculator_js { 
    extern const unsigned char qmlData[];
    extern const QQmlPrivate::AOTCompiledFunction aotBuiltFunctions[];
    const QQmlPrivate::CachedQmlUnit unit = {
        reinterpret_cast<const QV4::CompiledData::Unit*>(&qmlData), &aotBuiltFunctions[0], nullptr
    };
}

}
namespace {
struct Registry {
    Registry();
    ~Registry();
    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;
    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, unitRegistry)


Registry::Registry() {
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/Main.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_Main_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/ApplicationState.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_ApplicationState_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/Display.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_Display_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/NumberPad.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_NumberPad_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/CalculatorButton.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_CalculatorButton_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/BackspaceButton.qml"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_BackspaceButton_qml::unit);
    resourcePathToCachedUnit.insert(QStringLiteral("/qt/qml/demos/calqlatr/content/calculator.js"), &QmlCacheGeneratedCode::_qt_qml_demos_calqlatr_content_calculator_js::unit);
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.structVersion = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

Registry::~Registry() {
    QQmlPrivate::qmlunregister(QQmlPrivate::QmlUnitCacheHookRegistration, quintptr(&lookupCachedUnit));
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url) {
    if (url.scheme() != QLatin1String("qrc"))
        return nullptr;
    QString resourcePath = QDir::cleanPath(url.path());
    if (resourcePath.isEmpty())
        return nullptr;
    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));
    return unitRegistry()->resourcePathToCachedUnit.value(resourcePath, nullptr);
}
}
int QT_MANGLE_NAMESPACE(qInitResources_qmlcache_calqlatr)() {
    ::unitRegistry();
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(QT_MANGLE_NAMESPACE(qInitResources_qmlcache_calqlatr))
int QT_MANGLE_NAMESPACE(qCleanupResources_qmlcache_calqlatr)() {
    return 1;
}
