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

#ifndef REGEXABOUTDIALOG_H
#define REGEXABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class RegExAboutDialog;
}

namespace Nedrysoft {
    /**
     * @brief               RegExAboutDialog class
     *
     * @details             A dialog box that shows details about the application.
     */
    class RegExAboutDialog : public QDialog {
        private:
            Q_OBJECT

        public:
            /**
             * @brief           Constructs an about dialog.
             *
             * @param[in]       parent is the the owner of the dialog.
             */
            explicit RegExAboutDialog(QWidget *parent = nullptr);

            /**
             * @brief           Destroys the dialog.
             */
            ~RegExAboutDialog();

        protected:
            /**
             * @brief           Handles events for the dialog box.
             *
             * @details         To allow the dialog box to be dismissed easily, event processing at a more
             *                  granular level needs to occur.
             *
             * @param[in]       event contains information about the event being processed.
             * @returns         true if the event was handled; otherwise, false.
             */
            virtual bool event(QEvent *event) override;

            /**
             * @brief           Handles focus out events.
             *
             * @details         If the about dialog loses focus, this function is called to dismiss the dialog.
             *
             * @param[in]       event contains information about the focus event.
             */
            virtual void focusOutEvent(QFocusEvent *event) override;

        private:
            Ui::RegExAboutDialog *ui;                   //! ui class for the about dialog
    };
}
#endif // REGEXABOUTDIALOG_H
