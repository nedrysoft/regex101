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

#include <QJsonObject>
#include <QObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QVariant>

namespace Nedrysoft {
     class RegExDatabase;

    /**
     * @brief               RegExApiEndpoint class
     *
     * @details             Provides functions that can be called from javascript in the web browser.
     */
    class RegExApiEndpoint : public QObject {
        private:
            Q_OBJECT

        private:
            /**
             * @brief           Constructs an api endpoint class.
             *
             * @note            The constructor is private and the api endpoint is a singleton object
             *                  that should be retrieved using the getInstance() method.
             */
            RegExApiEndpoint();

        public:
            /**
             * @brief           Constructs an api endpoint class.
             */
            static RegExApiEndpoint *getInstance();

            /**
             * @brief           Retrieves an expression from storage
             *
             * @details         Finds the expression with the given permalink and version number and returns
             *                  the expression details as a variant map.
             *
             * @param[in,out]   state is a json object containing the current web application state
             * @param[in]       permalinkFragment the permalink fragment id
             * @param[in]       version contains the version number of the expression to load
             *
             * @returns         true if the result was found; otherwise false.
             */
            bool regex(QJsonObject &state, QString permalinkFragment, int version);

            /**
             * @brief           Method for fetching data from a web page.
             *
             * @details         This function uses the same signature as the fetch api provided by the web browser,
             *                  javascript is injected into the web page to replace the web browsers implementation
             *                  with this custom version which allows us to reply without having to make real network requests.
             *
             * @param[in]       pathParameter contains the path of the fetch request.
             * @param[in]       requestParameter contains the requesst parameters send via javascript.
             *
             * @returns         a QVariant containing the body of the response.
             */
            Q_INVOKABLE QVariant fetch(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Method setting a localstorage value
             *
             * @details         This function sets the storage item identified by the parameter key to the given value,
             *                  the injected javascript overrides the native implmentation of the local storage api
             *                  so that we have full control over it.
             *
             * @param[in]       key contains the key of item.
             * @param[in]       value contains the value of the item.
             *
             * @returns         a null QVariant
             */
            Q_INVOKABLE QVariant localStorageSetItem(const QVariant &key, const QVariant &value);

            /**
             * @brief           Method retrieving a localstorage value
             *
             * @details         This function retrieves the storage item identified by the parameter key,
             *                  the injected javascript overrides the native implmentation of the local storage api
             *                  so that we have full control over it.
             *
             * @param[in]       key contains the key of item.
             *
             * @returns         the value if set; otherwise null
             */
            Q_INVOKABLE QVariant localStorageGetItem(const QVariant &key);

            /**
             * @brief           Method removing a localstorage value
             *
             * @details         This function removes the storage item identified by the parameter key,
             *                  the injected javascript overrides the native implmentation of the local storage api
             *                  so that we have full control over it.
             *
             * @param[in]       key contains the key of item.
             *
             * @returns         a null QVariant
             */
            Q_INVOKABLE QVariant localStorageRemoveItem(const QVariant &key);

            /**
             * @brief           Method clearing the localstorage
             *
             * @details         This function clears the local storage.  The injected javascript overrides the native implmentation
             *                  of the local storage api so that we have full control over it.
             *
             * @param[in]       key contains the key of item.
             *
             * @returns         a null QVariant
             */
            Q_INVOKABLE QVariant localStorageClear();

            /**
             * @brief           Method for sending message from javascript to c++
             *
             * @param[in]       message contains the message received.
             */
            Q_INVOKABLE void notifyApplication(QVariant message) const;
        private:

            /**
             * @brief           Creates a random string of characters
             *
             * @details         Creates a string which is made of random characters from the set (a-z, A-z, 0-9)
             *
             * @param[in]       numberOfCharacters is the desired length of the string
             *
             * @returns         a string of the desired length consisting of random characters
             */
            QString createRandomString(int numberOfCharacters) const;

            /**
             * @brief           Creates a permalink fragment
             *
             * @details         Creates a string which is made of random characters from the set (a-z, A-z, 0-9)
             *
             * @returns         a string containing the permalink fragment
             */
            QString createPermalinkFragment() const;

            /**
             * @brief           Creates a deletecode
             *
             * @details         Creates a string which is made of random characters from the set (a-z, A-z, 0-9)
             *
             * @returns         a string containing the deletecode fragment
             */
            QString createDeleteCode() const;

            /**
             * @brief           Processes HTTP GET request
             *
             * @details         Any GET request that is made to the api endpoint is handled with this function
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes HTTP PUT request
             *
             * @details         Any PUT request that is made to the api endpoint is handled with this function
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processPutRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes HTTP POST request
             *
             * @details         Any POST request that is made to the api endpoint is handled with this function
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processPostRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes HTTP DELETE request
             *
             * @details         Any DELETE request that is made to the api endpoint is handled with this function
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processDeleteRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes a request made to the save endpoint
             *
             * @details         Function for handling a request made to the save api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processSaveRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes a request made to the fork endpoint
             *
             * @details         Function for handling a request made to the save api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processForkRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Processes a request made to the get library items endpoint
             *
             * @details         Function for handling a request made to the save api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetLibraryItems(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Processes a request made to the get items endpoint
             *
             * @details         Function for handling a request made to the save api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetItemDetails(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Processes a request made to the static endpoint
             *
             * @details         Function for handling a request made to the save api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetStatic(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Processes a request made to the put history endpoint
             *
             * @details         Function for handling a request made to the put history endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processPutHistoryRequest(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Processes a request made to the get regex endpoint
             *
             * @details         Function for handling a request made to the get regex endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetRegEx(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Uploads a regular expression to the library
             *
             * @details         Function for handling a request made to the library api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             *
             * @returns         a QVariant response to the request
             */
            QVariant processUploadToLibraryRequest(const QVariant &pathParameter, const QVariant &requestParameter) const;

            /**
             * @brief           Sets or unsets a favourite
             *
             * @details         Function for handling a request made to the favourite api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processSetFavorite([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

            /**
             * @brief           Handles calls to the get history api endpoint
             *
             * @details         Function for handling a request made to the get history api endpoint
             *
             * @param[in]       pathParameter contains the path of the api request
             * @param[in]       requestParameter contains the request detail such as HTTP headers and body
             * @param[in]       match contains the regular expression that was used determine the endpoint
             *
             * @returns         a QVariant response to the request
             */
            QVariant processGetHistory([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, const QRegularExpressionMatch &match) const;

        private:
            QSettings *m_settings;                          //! settings object to store the web application local storage data
            RegExDatabase *m_database;                      //! the database
            QMap<QString, QString> m_librarySearchSortMap;  //! map containing mapping from application sort type to database field
    };
}

#endif // REGEXAPIENDPOINT_H
