#ifndef REGEXAPIENDPOINT_H
#define REGEXAPIENDPOINT_H

#include <QObject>

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
             * @returns     a string containing the body of the response.
             */
            Q_INVOKABLE QString fetch(const QVariant &pathParameter, const QVariant &requestParameter) const;
    };
}

#endif // REGEXAPIENDPOINT_H
