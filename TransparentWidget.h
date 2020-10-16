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

#ifndef TRANSPARENTWIDGET_H
#define TRANSPARENTWIDGET_H

#include <QGridLayout>
#include <QWidget>

class QGraphicsOpacityEffect;

namespace Nedrysoft {
    /**
     * @brief               TransparentWidget class
     *
     * @details             QWidget subclass with support for transparency
     */
    class TransparentWidget :
            public QWidget
    {
        private:
            Q_OBJECT

        public:
            /**
             * @brief           Constructs a transparent widget
             *
             * @param[in]       childWidget is the widget to make transparent
             * @param[in]       opacity is the initial transparency level, 0 is completely transparent, 1 is completely opaque
             * @param[in]       parent is the the parent of the widget
             */
            TransparentWidget(QWidget *childWidget=nullptr, double opacity=1, QWidget *parent=nullptr);

            /**
             * @brief           Returns the transparency graphics effect
             *
             * @returns         returns the transparency effoct so that it can be used by property animation
             */
            QGraphicsOpacityEffect *transparencyEffect();

            /**
             * @brief           Sets the transparency value
             *
             * @param[in]       value is the transparency level, 0 is completely transparent, 1 is completely opaque
             */
            void setTransparency(double value);

        private:
            QGraphicsOpacityEffect *m_transparencyEffect;               //! the transparency graphics effect set to this widget
            QGridLayout *m_layout;                                      //! layout used on the transparent window
            QWidget *m_childWidget;                                     //! the child widget being made transparent
    };
}

#endif // TRANSPARENTWIDGET_H
