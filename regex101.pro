#
# Copyright (C) 2020 Adrian Carpenter
#
# This file is part of a regex101.com offline application.
#
# https://github.com/fizzyade/regex101
#
# =====================================================================
# The regex101 web content is owned and used with permission from
# Firas Dib at regex101.com.  This application is an unofficial
# tool to provide an offline application version of the website.
# =====================================================================
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# macOS deployment
# ----------------
#
# ./deploy.py --appleid="adrian.carpenter@me.com" \
#             --password="@keychain:signingpassword" -\
#             --cert="Developer ID Application: Adrian Carpenter (5JN9ZQ38G6)" \
#             --arch=x86_64 \
#             --type=release \
#             --qtdir="/Users/adriancarpenter/Qt/5.15.0/clang_64"
#
# Windows deployment
# ------------------
#
# python deploy.py --curlbin="C:\Tools\curl\bin\curl.exe" ^
#                  --qtdir="C:\Qt\5.14.2\msvc2017_64" ^
#                  --type=release ^
#                  --cert="Open Source Developer, Adrian Carpenter"
#
# Linux deployment
# ----------------
#
# ./deploy.py --qtdir="/home/adrian/Qt/5.14.2/gcc_64" \
#             --curlbin="/usr/bin/curl” \
#             -—cert=05DDE1C3F3B50B53112A13C231A0DBA53EEA9FE6

TARGET = "Regular Expressions 101"

CONFIG(release, debug|release) {
    DESTDIR = $$PWD/bin/$$QT_ARCH/Release
} else {
    DESTDIR = $$PWD/bin/$$QT_ARCH/Debug
}

QT += \
    core \
    gui \
    widgets \
    webenginewidgets \
    webchannel

CONFIG += \
    c++11 \
    app_bundle

CONFIG -= \
    qtquickcompiler

SOURCES += \
    RegExAboutDialog.cpp \
    RegExApiEndpoint.cpp \
    RegExNullWebEnginePage.cpp \
    RegExSplashScreen.cpp \
    RegExUrlRequestInterceptor.cpp \
    RegExUrlSchemeHandler.cpp \
    RegExWebEnginePage.cpp \
    RegExWebEngineProfile.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h \
    RegExAboutDialog.h \
    RegExApiEndpoint.h \
    RegExNullWebEnginePage.h \
    RegExSplashScreen.h \
    RegExUrlRequestInterceptor.h \
    RegExUrlSchemeHandler.h \
    RegExWebEnginePage.h \
    RegExWebEngineProfile.h

FORMS += \
    MainWindow.ui \
    RegExAboutDialog.ui

# embedded resources - regex101.com web files & application images

RESOURCES += \
    regex101.qrc

# macOS application bundle icon

ICON = \
    regex101.icns

# windows executable icon

RC_ICONS = \
    icon.ico
