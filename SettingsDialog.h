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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>

#if defined(Q_OS_MACOS)
class QMacToolBar;
#endif

namespace Nedrysoft {
    class TransparentWidget;
    /**
     * @brief               Settings dialog class
     *
     * @details             Provides the settings dialog to modify preferences.  On macOS the dialog
     *                      is presented in a native style, under Windows & Linux the dialog uses a
     *                      format which is consistent and has a standard appearance for those OS's.
     */
    class SettingsDialog : public QWidget
    {
        Q_OBJECT

        public:
            /**
             * @brief           Constructs a settings window
             *
             * @param[in]       parent is the the owner of the window.
             */
            explicit SettingsDialog(QWidget *parent=nullptr);

        private:
            /**
             * @brief           Returns the QWindow handle from native widget
             *
             * @returns         The QWindow handle of the native window
             */
            QWindow *nativeWindowHandle();

        protected:
            /**
             * @brief           Window resize event
             *
             * @details         Called when the widget is resized, allows child widgets to be manually resized
             *
             * @params[in]      event is the event containing the resize information
             */
            virtual void resizeEvent(QResizeEvent *event);

        private:
#if defined(Q_OS_MACOS)
            QMacToolBar *m_toolBar;                         //! A native macOS toolbar (unified style)
#endif
            QList<TransparentWidget *> m_widgets;           //! The list of settings widgets
    };
}

#endif // SETTINGSDIALOG_H
