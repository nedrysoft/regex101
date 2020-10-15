/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of a regex101.com offline application.
 *
 * https://github.com/fizzyade/regex101
 *
 * =====================================================================
 * The regex101 web content is owned and used with permission from
 * Firas Dib at regex101.com.  This application is an unofficial
 * tool to provide an offline application version of the website.
 * =====================================================================
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "RegExSplashScreen.h"
#include "RegExUrlSchemeHandler.h"
#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QFont>
#include <QFontDatabase>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <stdio.h>
#include <stdlib.h>

constexpr auto applicationName = APPLICATION_LONG_NAME;                     //! Provided by CMake to the preprocessor
constexpr auto applicationFontsPrefix = ":/fonts";                          //! Fonts are stored under :/fonts (recursive search is performed)

void regexMessageHandler([[maybe_unused]] QtMsgType type, [[maybe_unused]] const QMessageLogContext &context, const QString &msg)
{
    fprintf(stdout, "%s\r\n", msg.toLatin1().data());
}

int main(int argc, char *argv[])
{
    //qInstallMessageHandler(regexMessageHandler);

    Nedrysoft::RegExUrlSchemeHandler::registerScheme();
    QMimeDatabase mimeDatabase;
    QApplication application(argc, argv);

#if defined(Q_OS_WINDOWS)
    QGuiApplication::setWindowIcon(QIcon(":/assets/regex101.iconset/icon_1024x1024@2x.png"));
#endif
    // search the /fonts folder in the resources and attempt to load any found fonts

    auto fontDirIterator = QDirIterator(applicationFontsPrefix, QDirIterator::Subdirectories);

    while(fontDirIterator.hasNext())
    {
        fontDirIterator.next();

        auto mimeType = mimeDatabase.mimeTypeForFile(fontDirIterator.filePath()).name();

        if (QRegularExpression(R"(font\/.*)").match(mimeType).hasMatch()) {
            QFontDatabase::addApplicationFont(fontDirIterator.filePath());
        }
    }

    Nedrysoft::RegExSplashScreen splashScreen;

    application.setApplicationDisplayName(applicationName);
    application.setApplicationName(applicationName);

    auto mainWindow = new Nedrysoft::MainWindow(&splashScreen);

    auto returnValue = application.exec();

    delete mainWindow;

    return returnValue;
}
