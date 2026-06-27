include("C:/CJOPROOQT/QT creator - calqlatr/build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/calqlatr-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase;qtdeclarative")

qt6_deploy_qml_imports(TARGET calqlatr PLUGINS_FOUND plugins_found)
qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/CJOPROOQT/QT creator - calqlatr/build/Desktop_Qt_6_11_1_MinGW_64_bit-Debug/calqlatr.exe"
    ADDITIONAL_MODULES ${plugins_found}
    GENERATE_QT_CONF
)