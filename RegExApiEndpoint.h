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

#ifndef REGEXAPIENDPOINT_H
#define REGEXAPIENDPOINT_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>

namespace Nedrysoft {
    /**
     * @brief           RegExApiEndpoint class
     *
     * @details         Provides functions that can be called from javascript in the web browser.
     */
    class RegExApiEndpoint : public QObject {
        Q_OBJECT

        public:
            /**
             * @brief       Constructs an api endpoint class.
             */
            RegExApiEndpoint();

            /**
             * @brief       Method for fetching data from a web page.
             *
             * @details     This function uses the same signature as the fetch api provided by the web browser,
             *              javascript is injected into the web page to replace the web browsers implementation
             *              with this custom version which allows us to reply without having to make real network requests.
             *
             * @param[in]   pathParameter contains the path of the fetch request.
             * @param[in]   requestParameter contains the requesst parameters send via javascript.
             *
             * @returns     a QVariant containing the body of the response.
             */
            Q_INVOKABLE QVariant fetch(const QVariant &pathParameter, const QVariant &requestParameter) const;

        private:
            QSqlDatabase m_database;
    };
}

#endif // REGEXAPIENDPOINT_H
