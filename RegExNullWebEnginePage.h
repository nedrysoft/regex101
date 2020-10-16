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

#ifndef REGEXNULLWEBENGINEPAGE_H
#define REGEXNULLWEBENGINEPAGE_H

#include <QWebEnginePage>

namespace Nedrysoft {
    /**
     * @brief               RegExNullWebEnginePage class
     *
     * @details             Provides a web engine page that is used to intercept clicked links in the main browser
     *                      window.  This is required to catch javascript link clicks with target="_blank" as the
     *                      link target.  This class denies the navigation request, but uses the qt desktop services
     *                      to open the link in the system default web browser.
     */
    class RegExNullWebEnginePage :
            public QWebEnginePage
    {
        private:
            Q_OBJECT

        public:
            /**
             * @brief           Constructs a null web page.
             */
            RegExNullWebEnginePage();

        protected:
            /**
             * @brief           acceptNavigationRequest
             *
             * @details         Allows or blocks requests that are send by the page
             *
             * @param[in]       url is the url to be navigated to.
             * @param[in]       type is the type of request, click etc.
             * @param[in]       isMainFrame is true if this is the main browser; otherwise false.
             *
             * @returns         true if the navigation is allowed; otherwise false.
             */
            bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
    };
}
#endif // REGEXNULLWEBENGINEPAGE_H
