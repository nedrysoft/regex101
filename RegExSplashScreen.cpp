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

#include "RegExSplashScreen.h"
#include <QLabel>
#include <QPainter>

constexpr auto splashScreenFilename = ":/assets/splash_620x375@2x.png";
constexpr auto fontFamily = "Open Sans";
constexpr auto fontSize = 14;

Nedrysoft::RegExSplashScreen::RegExSplashScreen() :
    QSplashScreen(QPixmap(splashScreenFilename), Qt::WindowStaysOnTopHint)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    show();
}

Nedrysoft::RegExSplashScreen::~RegExSplashScreen()
{
}

void Nedrysoft::RegExSplashScreen::drawContents(QPainter *painter)
{
    auto font = QFont(fontFamily, fontSize, QFont::Weight::Normal);
    auto versionText = QString("%1.%2.%3 (%4 %5)").arg(APPLICATION_GIT_YEAR).arg(APPLICATION_GIT_MONTH).arg(APPLICATION_GIT_DAY).arg(APPLICATION_GIT_BRANCH).arg(APPLICATION_GIT_HASH);

    QSplashScreen::drawContents(painter);

    painter->save();

    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::white);
    painter->setFont(font);

    painter->drawText(QRect(350, 300, 250, 71), Qt::AlignRight | Qt::AlignVCenter, versionText);

    painter->restore();
}
