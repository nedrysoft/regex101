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

#ifndef ISETTINGSPAGE_H
#define ISETTINGSPAGE_H

namespace Nedrysoft {
    /**
     * @brief               Interface for a settings page
     *
     * @details             Describes the interface contract that a settings page must adhere to.
     */
    class ISettingsPage
    {
        public:
            /**
             * @brief               Checks if the settings can be applied
             *
             * @returns             true if the settings can be applied (i.e valid); otherwise false.
             */
            virtual bool canAcceptSettings() = 0;

            /**
             * @brief               Applys the current settings
             */
            virtual void acceptSettings() = 0;
    };
}

#endif
