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

#ifndef CLONER_H
#define CLONER_H

namespace Nedrysoft {
    /**
     * @brief               Clones the regex101.com
     *
     * @details             Downloads all the required files from the regex101.com site,
     *                      this involves searching through each file that is downloaded to find any
     *                      other dependencies that are required.
     *
     *                      The purpose of this is to allow users to update to a later verison of the
     *                      regex101.com application, although they do this at their own risk as newer version
     *                      may have changes that break the url handler.
     */
    class Cloner
    {
        public:
            /**
             * @brief               Constructor
             *
             * @details             Constructs a Cloner instance
             */
            Cloner();
    };
}

#endif // CLONER_H
