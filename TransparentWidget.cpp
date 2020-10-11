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

#include "TransparentWidget.h"
#include <QGraphicsOpacityEffect>

Nedrysoft::TransparentWidget::TransparentWidget(QWidget *childWidget, double opacity, QWidget *parent) :
    QWidget(parent)
{
    m_transparencyEffect = new QGraphicsOpacityEffect(this);

    m_layout = new QGridLayout;

    m_layout->setMargin(0);

    m_childWidget = childWidget;

    setLayout(m_layout);

    if (childWidget) {
        m_layout->addWidget(childWidget);
    }

    connect(m_transparencyEffect, &QGraphicsOpacityEffect::opacityChanged, [=](qreal opacity) {
        if (opacity) {
            setVisible(true);
        } else {
            setVisible(false);
        }
    });

    m_transparencyEffect->setOpacity(opacity);

    setGraphicsEffect(m_transparencyEffect);

    if (opacity==0) {
        hide();
    }
}

QGraphicsOpacityEffect *Nedrysoft::TransparentWidget::transparencyEffect()
{
    return m_transparencyEffect;
}

void Nedrysoft::TransparentWidget::setTransparency(double value)
{
    m_transparencyEffect->setOpacity(value);
}
