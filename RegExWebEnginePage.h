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

#ifndef REGEXWEBENGINEPAGE_H
#define REGEXWEBENGINEPAGE_H

#include "RegExApiEndpoint.h"
#include "RegExUrlRequestInterceptor.h"
#include "RegExWebEngineProfile.h"
#include <QWebEnginePage>

class QWebChannel;

namespace Nedrysoft {
    /**
     * @brief           RegExWebEnginePage class.
     *
     * @details         This subclass provides a web page that is set up to use the handlers,
     *                  interceptors and web channels.
     */
    class RegExWebEnginePage : public QWebEnginePage {
        Q_OBJECT

        public:
            /**
             * @brief       Constructs a web engine page.
             */
            RegExWebEnginePage();

            /**
             * @brief       Destroys the web engine page.
             */
            ~RegExWebEnginePage();

        protected:
            /**
             * @brief       javaScriptConsoleMessage
             *
             * @details     This overridden function allows debug information from the console.log functions
             *              to be obtained.
             *
             * @param[in]   level is the level of message.
             * @param[in]   message is the content of the message sent via javascript.
             * @param[in]   lineNumber is the number of the line that caused the output.
             * @param[in]   sourceID is the identifier of the source of the message.
             */
            virtual void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID);

        private:
            Nedrysoft::RegExUrlRequestInterceptor *m_urlInterceptor;
            RegExWebEngineProfile *m_profile;
            QWebChannel *m_apiChannel;
            Nedrysoft::RegExApiEndpoint *m_apiEndpoint;
    };
}

#endif // REGEXWEBENGINEPAGE_H
