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

#include "RegExAboutDialog.h"
#include "ui_RegExAboutDialog.h"

#include <QDebug>
#include <QDialog>
#include <QDesktopWidget>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QString>

constexpr auto splashScreenFilename = ":/assets/splash_620x375@2x.png";
constexpr auto fontFamily = "Open Sans";
constexpr auto fontSize = 14;

Nedrysoft::RegExAboutDialog::RegExAboutDialog(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint)
{
    m_backgroundPixmap = QPixmap(splashScreenFilename);

    auto dialogSize = (QSizeF(m_backgroundPixmap.size())/m_backgroundPixmap.devicePixelRatioF());

    resize((dialogSize.toSize()));

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
}

void Nedrysoft::RegExAboutDialog::focusOutEvent([[maybe_unused]] QFocusEvent *event)
{
    close();
}

bool Nedrysoft::RegExAboutDialog::event(QEvent *event)
{
    switch(event->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::KeyPress: {
            close();
            break;
        }

        default: {
            break;
        }
    }

    return QDialog::event(event);
}

Nedrysoft::RegExAboutDialog::~RegExAboutDialog()
{
}

void Nedrysoft::RegExAboutDialog::paintEvent(QPaintEvent *paintEvent)
{
    QDialog::paintEvent(paintEvent);

    auto font = QFont(fontFamily, fontSize, QFont::Weight::Normal);
    auto versionText = QString("%1.%2.%3 (%4 %5)").arg(APPLICATION_GIT_YEAR).arg(APPLICATION_GIT_MONTH).arg(APPLICATION_GIT_DAY).arg(APPLICATION_GIT_BRANCH).arg(APPLICATION_GIT_HASH);

    QPainter painter(this);

    painter.save();

    painter.drawPixmap(0, 0, m_backgroundPixmap);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.setFont(font);

    painter.drawText(QRect(350, 300, 250, 71), Qt::AlignRight | Qt::AlignVCenter, versionText);

    painter.restore();
}
