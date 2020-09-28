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

/**
 * @brief       Creates the web channel to communicate with the backend.
 *
 * @details     This is injected into the web page and is used to construct a web channel
 *              which connects to the C++.  When the web channel is opened, the
 *              api endpoint is returned and the webapi fetch function is replaced with
 *              the custom one provided by the web channel.
 */
window.webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
    window.apiEndPoint = channel.objects.RegExApiEndpoint;
    window.fetch = regexApiFetch
});

/**
 * @brief       Provides a webapi fetch style function
 *
 * @details     To provide full support for the regex101 application, the webapi fetch command
 *              has to be replaced with something functionally similar that can directly talk
 *              to the C++ application, this implemention provides that bridge.
 *
 * @param[in]   path is the path to the file.
 * @param[in]   request is the data to be sent in the request.
 *
 * @returns     a promise object
 */
function regexApiFetch(path, request)
{
   return new Promise(function(resolve, reject) {
        return window.apiEndPoint.fetch(path, request).then(function(e) {
            var response = {
                status: 200,
                json : function() {
                    return JSON.parse(e);
                }
            };

            resolve(response);
        });
    });
}
