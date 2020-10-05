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
 * @brief       Javascript overridden functions
 *
 * @details     In order to make the original regex101.com site work correctly as a standalone application
 *              various built-in javascript functions are replaced with our own implementations which allows
 *              the C++ application to interact with the web application, this includes the local storage api
 *              which is replaced with an implementation that allows the C++ applicaiton to handle storage of
 *              data rather than the web engine.
 *
 *              This javascript file is injected into requests that the web applicaiton makes.
 */

console.log("running code")

window.fetch = null;

window.localStorage.setItem = function (key, value) {
    console.log('setItem '+key+' '+value)
};

window.localStorage.removeItem = function (key) {
    console.log('removeItem '+key)
};

window.localStorage.getItem = function (key) {
    return "{}"
};

window.localStorage.clear = function () {
    console.log('clear')
};

if (typeof window.nedrysoftWebChannel=="undefined") {
    /**
     * @brief       Creates the web channel to communicate with the backend.
     *
     * @details     Creates and iniitialises a QWebChannel object which allows access
     *              to C++ functions from javascript.
     *
     * @notes       The resulting web channel object is stored in window
     */
     window.nedrysoftWebChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
        window.nedrysoftApiEndPoint = channel.objects.RegExApiEndpoint;
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
         window.fetch = fetch = function(path, request) {
            return new Promise(function(resolve, reject) {
                 return window.nedrysoftApiEndPoint.fetch(path, request).then(function(e) {
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

         /**
          * @brief           Store item in local storage
          *
          * @details         Overrides the default implementation of the local storage api and redirects requests to
          *                  the C++ implementation.
          *
          * @param[in]       key is the identifier of the data item to set
          * @param[in]       value is the data to be stored by the given key
          *
          * @returns         An empty JSON object
          */
         window.localStorage.setItem = function (key, value) {
             return JSON.stringify(window.nedrysoftApiEndPoint.localStorageSetItem(key, value))
         }

         /**
          * @brief           Retrieve item from local storage
          *
          * @details         Overrides the default implementation of the local storage api and redirects requests to
          *                  the C++ implementation.
          *
          * @param[in]       key is the identifier of the data item to retrieve
          *
          * @returns         A string containing a JSON representation of the stored object
          */
         window.localStorage.getItem = function(key) {
             return JSON.stringify(window.nedrysoftApiEndPoint.localStorageGetItem(key))
         }

         /**
          * @brief           Clear local storage
          *
          * @details         Overrides the default implementation of the local storage api and redirects requests to
          *                  the C++ implementation.  This function clears the local storage.
          *
          * @returns         An empty JSON object
          */
         window.localStorage.clear = function() {
             return JSON.stringify(window.nedrysoftApiEndPoint.regexApiLocalStorageClear())
         }

         /**
          * @brief           Remove a specific item from local storage
          *
          * @details         Overrides the default implementation of the local storage api and redirects requests to
          *                  the C++ implementation.
          *
          * @param[in]       key is the identifier of the data item to remove
          *
          * @returns         An empty JSON object
          */
         window.localStorage.removeItem = function(key) {
             return JSON.stringify(window.nedrysoftApiEndPoint.regexApiLocalStorageRemoveItem(key))
         }
/*
         // Inform application that the injected javascript has finished initialisation

         window.nedrysoftApiEndPoint.notifyApplication("injection started.");

         // !!!NEDRYSOFT_INJECT_FILE!!!

         window.nedrysoftApiEndPoint.notifyApplication("injection ended.");

         var evt = document.createEvent('Event');

         evt.initEvent('load', false, false);

         window.dispatchEvent(evt);

         window.nedrysoftApiEndPoint.notifyApplication("load event triggered.");*/
     });
 } /*else {
    window.nedrysoftApiEndPoint.notifyApplication("injection started.");

     // !!!NEDRYSOFT_INJECT_FILE!!!

    window.nedrysoftApiEndPoint.notifyApplication("injection ended.");
 }
*/

