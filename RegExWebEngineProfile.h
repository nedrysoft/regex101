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

#ifndef REGEXWEBENGINEPROFILE_H
#define REGEXWEBENGINEPROFILE_H

#include <QWebEngineProfile>
#include "RegExUrlSchemeHandler.h"

namespace Nedrysoft {
    /**
     * @brief               RegExWebEngineProfile class
     *
     * @details             Provides a profile which is set up to handle the custom scheme with
     *                      appropriate security levels.
     */
    class RegExWebEngineProfile : public QWebEngineProfile {
        private:
            Q_OBJECT

        public:
            /**
             * @brief           Constructs a web engine profile.
             *
             * @param[in]       parent is the the owner of the profile.
             */
            RegExWebEngineProfile(QObject *parent=nullptr);

        private:
            Nedrysoft::RegExUrlSchemeHandler *m_schemeHandler;                  //! pointer to the regex101: scheme handler object
    };
}

#endif // REGEXWEBENGINEPROFILE_H
