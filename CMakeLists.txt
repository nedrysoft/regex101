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

if((APPLE) AND (CMAKE_OSX_ARCHITECTURES MATCHES "arm64"))
    set(APPLICATION_PLATFORM_ARCH "arm64")
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(APPLICATION_PLATFORM_ARCH "x86_64")
    else()
        set(APPLICATION_PLATFORM_ARCH "x86_32")
    endif()
endif()

set(APPLICATON_SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

set(APPLICATION_BIN_OUTPUT "${CMAKE_CURRENT_LIST_DIR}/bin/${APPLICATION_PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The list of source files

set(APPLICATION_SOURCES
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
    Info.plist.in
)

# List of Qt libraries used by the application

set(APPLICATION_QT_LIBRARIES
    Core
    Gui
    Sql
    WebEngineWidgets
    WebChannel
    Widgets
)

# Check that the Qt libraries are available

find_package(Qt5 COMPONENTS ${APPLICATION_QT_LIBRARIES} REQUIRED)

# use Git to produce version information and pass into compiler

find_package(Git REQUIRED)

execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git log -1 --format=%cd --date=format:%Y OUTPUT_VARIABLE APPLICATION_GIT_YEAR)
execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git log -1 --format=%cd --date=format:%m OUTPUT_VARIABLE APPLICATION_GIT_MONTH)
execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git log -1 --format=%cd --date=format:%d OUTPUT_VARIABLE APPLICATION_GIT_DAY)
execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git log -1 --format=%h OUTPUT_VARIABLE APPLICATION_GIT_HASH)
execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git branch --show-current OUTPUT_VARIABLE APPLICATION_GIT_BRANCH)
execute_process(COMMAND git --git-dir=${APPLICATON_SOURCE_ROOT}/.git diff --quiet HEAD RESULT_VARIABLE APPLICATION_GIT_UNCOMMITED)

if (APPLICATION_GIT_UNCOMMITED EQUAL 1)
    set(APPLICATION_GIT_UNCOMMITED "+")
else()
    set(APPLICATION_GIT_UNCOMMITED "")
endif()

if ("${APPLICATION_GIT_YEAR}" STREQUAL "")
    set(APPLICATION_GIT_YEAR 0)
    set(APPLICATION_GIT_MONTH 0)
    set(APPLICATION_GIT_DAY 0)
    set(APPLICATION_GIT_BRANCH none)
    set(APPLICATION_GIT_HASH 0)
else()
    string(STRIP "${APPLICATION_GIT_YEAR}" APPLICATION_GIT_YEAR)
    string(STRIP "${APPLICATION_GIT_MONTH}" APPLICATION_GIT_MONTH)
    string(STRIP "${APPLICATION_GIT_DAY}" APPLICATION_GIT_DAY)
    string(STRIP "${APPLICATION_GIT_BRANCH}" APPLICATION_GIT_BRANCH)
    string(STRIP "${APPLICATION_GIT_HASH}" APPLICATION_GIT_HASH)
endif()

add_definitions("-DAPPLICATION_GIT_YEAR=\"${APPLICATION_GIT_YEAR}\"")
add_definitions("-DAPPLICATION_GIT_MONTH=\"${APPLICATION_GIT_MONTH}\"")
add_definitions("-DAPPLICATION_GIT_DAY=\"${APPLICATION_GIT_DAY}\"")
add_definitions("-DAPPLICATION_GIT_BRANCH=\"${APPLICATION_GIT_BRANCH}\"")
add_definitions("-DAPPLICATION_GIT_HASH=\"${APPLICATION_GIT_HASH}\"")

# Generate the list of Qt libraries to link into the application

set(APPLICATION_QT_LINK_LIBRARIES "")

foreach(arg IN ITEMS ${APPLICATION_QT_LIBRARIES})
    list(APPEND APPLICATION_QT_LINK_LIBRARIES "Qt5::${arg}")
endforeach()

# Create an application executable (and ensure it's an application bundle under macOS)

add_executable(regex101 MACOSX_BUNDLE ${APPLICATION_SOURCES})

# Set the target executable name

set_target_properties(regex101 PROPERTIES OUTPUT_NAME "Regular Expressions 101")

# Set the folder where the binary should be placed

set_target_properties(regex101 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${APPLICATION_BIN_OUTPUT})

# If macOS build, then set the bundle icon and copy to rhe resulting application bundle

if(APPLE)
    #set(MACOSX_BUNDLE_ICON_FILE regex101.icns)

    set_source_files_properties(regex101.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(regex101 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${APPLICATON_SOURCE_ROOT}/Info.plist.in)

    set(APPLICATION_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
endif()

# Set the libraries be linked in

target_link_libraries(regex101 PRIVATE ${APPLICATION_QT_LINK_LIBRARIES})
