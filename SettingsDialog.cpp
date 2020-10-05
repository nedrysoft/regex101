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

#include "SettingsDialog.h"
#include "TransparentWidget.h"

#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QLabel>
#if defined(Q_OS_MACOS)
#include <QMacToolBar>
#endif
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>

constexpr auto transisionDuration = 100;

Nedrysoft::SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent)
{
#if defined(Q_OS_MACOS)
    m_toolBar = new QMacToolBar(this);
#endif

    resize(600,400);

    auto primary = new TransparentWidget(new QLabel("HELLO"), 1, this);
    auto secondary = new TransparentWidget(new QLabel("GOODBYE"), 0, this);

    m_widgets.append(primary);
    m_widgets.append(secondary);

    auto primaryEffect = primary->transparencyEffect();
    auto secondaryEffect = secondary->transparencyEffect();

#if defined(Q_OS_MACOS)
    QMacToolBarItem *toolBarItem = m_toolBar->addItem(QIcon(":/assets/regex101.iconset/icon_256x256@2x.png"), QStringLiteral("foo"));

    connect(toolBarItem, &QMacToolBarItem::activated, this, [this,primaryEffect,secondaryEffect]() {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");

        animation->setDuration(transisionDuration);
        animation->setStartValue(this->frameGeometry());
        animation->setEndValue(this->frameGeometry().adjusted(-100, -100, 100, 100));

        QParallelAnimationGroup *group = new QParallelAnimationGroup;

        group->addAnimation(animation);

        QPropertyAnimation *fo = new QPropertyAnimation(primaryEffect,"opacity");

        fo->setDuration(transisionDuration);
        fo->setStartValue(1);
        fo->setEndValue(0);
        group->addAnimation(fo);

        QPropertyAnimation *fi = new QPropertyAnimation(secondaryEffect,"opacity");

        fi->setDuration(transisionDuration);
        fi->setStartValue(0);
        fi->setEndValue(1);

        group->addAnimation(fi);

        group->start();
    });

    m_toolBar->attachToWindow(nativeWindowHandle());
#endif
}

void Nedrysoft::SettingsDialog::resizeEvent(QResizeEvent *event)
{
    for(auto widget : m_widgets) {
        widget->resize(event->size());
    }
}

QWindow *Nedrysoft::SettingsDialog::nativeWindowHandle()
{
    // @note the call to winId() is required as it sets up windowHandle() to return the correct value,
    //       failing to call this will result in windowHandle not returning the correct value.

    this->window()->winId();

    return this->window()->windowHandle();
}
