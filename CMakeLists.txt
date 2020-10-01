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

cmake_minimum_required(VERSION 3.5)

project("Regular Expressions 101" LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Sql WebEngineWidgets WebChannel Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Sql WebEngineWidgets WebChannel Widgets REQUIRED)

add_executable(regex101 MACOSX_BUNDLE
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    RegExAboutDialog.cpp
    RegExAboutDialog.h
    RegExAboutDialog.ui
    RegExApiEndpoint.cpp
    RegExApiEndpoint.h
    RegExNullWebEnginePage.cpp
    RegExNullWebEnginePage.h
    RegExSplashScreen.cpp
    RegExSplashScreen.h
    RegExUrlRequestInterceptor.cpp
    RegExUrlRequestInterceptor.h
    RegExUrlSchemeHandler.cpp
    RegExUrlSchemeHandler.h
    RegExWebEnginePage.cpp
    RegExWebEnginePage.h
    RegExWebEngineProfile.cpp
    RegExWebEngineProfile.h
    regex101.qrc
    regex101.icns
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(APPLICATION_PLATFORM_ARCH "x86_64")
else()
    set(APPLICATION_PLATFORM_ARCH "x86_32")
endif()


set_target_properties(regex101 PROPERTIES OUTPUT_NAME "Regular Expressions 101")
set_target_properties(regex101 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/${APPLICATION_PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}")

set(MACOSX_BUNDLE_ICON_FILE regex101.icns)
set_source_files_properties(regex101.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

target_link_libraries(regex101
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::WebChannel
    Qt${QT_VERSION_MAJOR}::Widgets
)
