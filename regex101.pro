QT       += core gui

TARGET = "Regular Expressions 101"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webenginewidgets

CONFIG += c++11
CONFIG += app_bundle

ICON = regex101.icns

SOURCES += \
    RegExSplashScreen.cpp \
    RegExUrlRequestInterceptor.cpp \
    RegExUrlSchemeHandler.cpp \
    RegExWebEnginePage.cpp \
    RegExWebEngineProfile.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h \
    RegExSplashScreen.h \
    RegExUrlRequestInterceptor.h \
    RegExUrlSchemeHandler.h \
    RegExWebEnginePage.h \
    RegExWebEngineProfile.h

FORMS += \
    MainWindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    regex101.qrc

RC_ICONS = icon.ico
