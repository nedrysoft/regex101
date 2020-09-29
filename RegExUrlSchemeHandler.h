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

#ifndef REGEXURLSCHEMEHANDLER_H
#define REGEXURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineProfile>

namespace Nedrysoft {
    /**
     * @brief           RegExUrlSchemeHandler class
     *
     * @details         This class provides a custom scheme implementation that allows content to be
     *                  displayed in a web page from a local data source.  This class is used in conjunction
     *                  with the RefExUrlRequestInterceptor and RegExApiEndpoint classes to provide
     *                  a full implementation.
     */
    class RegExUrlSchemeHandler : public QWebEngineUrlSchemeHandler {
        Q_OBJECT

        public:
            /**
             * @brief       Constructs a URL scheme handler.
             *
             * @param[in]   resourceRootFolder is the root path of the internal web content.
             */
            RegExUrlSchemeHandler(QString resourceRootFolder);

            /**
             * @brief       requestStarted
             *
             * @details     This function is called when a request is made to the custom scheme,
             *              the job includes information about the request and functions to
             *              tell the web engine whether or not the request was successful.
             *
             * @param[in]   request is the information about the request job.
             */
            void requestStarted(QWebEngineUrlRequestJob *request);

            /**
             * @brief       registerScheme
             *
             * @details     Registers the custom scheme with the web engine.
             *
             * @note        This must be called before the QApplication is instantiated.
             */
            static void registerScheme();

            /**
             * @brief       name
             *
             * @details     Returns the name used for the scheme.
             *
             * @returns     The name of the scheme.
             */
            static QString name();

            /**
             * @brief       scheme
             *
             * @details     Returns the scheme.
             *
             * @returns     the scheme.
             */
            static QString scheme();

            /**
             * @brief       root
             *
             * @details     Returns the scheme root url.
             *
             * @returns     the root url.
             */
            static QString root();
        private:
            QString m_resourceRootFolder;
    };
}

#endif // REGEXURLSCHEMEHANDLER_H
