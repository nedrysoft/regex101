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

#ifndef DATABASESETTINGSPAGE_H
#define DATABASESETTINGSPAGE_H

#include <QWidget>
#include "ISettingsPage.h"

namespace Ui {
    class DatabaseSettingsPage;
}

namespace Nedrysoft {
    /**
     * @brief               Database settings page
     *
     * @details             A widget that implements ISettingsPage to provide configuration options for
     *                      database settings.
     */
    class DatabaseSettingsPage :
            public QWidget,
            public ISettingsPage
    {
            Q_OBJECT

        public:
            /**
             * @brief               Constructor
             *
             * @details             Constructs a DatabaseSettingsPage instance
             *
             * @param[in]           parent is the parent widget (this is used to calculate the position of the window)
             */
            explicit DatabaseSettingsPage(QWidget *parent = nullptr);

            /**
             * @brief               Destructor
             *
             * @details             Destroys the instance
             */
            ~DatabaseSettingsPage();

            /**
             * @sa                  ISettingsPage::canAcceptSettings();
             */
            bool canAcceptSettings();

            /**
             * @sa                  ISettingsPage::acceptSettings();
             */
            void acceptSettings();

        protected:
            /**
             * @brief               Size hint
             *
             * @details             Provides a size hint for the widget.
             *
             * @returns             A QSize containting the size hint information
             */
            QSize sizeHint() const;

        private:
            Ui::DatabaseSettingsPage *ui;               //! ui class

            QSize m_size;                               //! the calculated size of the widget
    };
}

#endif // DATABASESETTINGSPAGE_H
