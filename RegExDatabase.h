#ifndef REGEXDATABASE_H
#define REGEXDATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantMap>

namespace Nedrysoft
{
    class RegExDatabase
    {
        private:
            /**
             * @brief           Constructs a RegExDatabase
             *
             * @note            Constructor is private and interaction with the class should be performed
             *                  via the singleton intance created and returned in the getInstance method.
             */
            RegExDatabase();

        public:
            /**
             * @brief           Returns the singleton instance of the class
             *
             * @details         Constructor of class is not public and access is performed through
             *                  the instance returned here.
             *
             * @returns         the singleton instance of the object
             */
            static RegExDatabase *getInstance();

            /**
             * @brief           Initialise a new database
             *
             * @details         Creates the required tables for the database.
             *
             * @returns         true on success; otherfalse false
             */
            bool initialiseDatabase();

            /**
             * @brief           Prepares a SQL query stored in the resources
             *
             * @details         Allows SQL queries to be stored in the resources file under /sql", the
             *                  queries can be written as formatted text and then loaded simply using this function.
             *
             * @param[in]       queryName the name of the query (no prefix or extension)
             *
             * @returns         the prepared query
             */
            QSqlQuery prepareQuery(QString queryName);

            /**
             * @brief           Returns the database configuration
             *
             * @details         Returns the parameters for the database connection in a variant map
             *
             * @returns         a variant map containing the configuration
             */
            QVariantMap getSettings();

            /**
             * @brief           Stores the database configuration
             *
             * @details         Stores the configuration for the database
             *
             * @param[in]       settingsMap is the map containing the configuration keys and values
             */
            void storeSettings(QVariantMap settingsMap);

        private:
            QSqlDatabase m_database;                        //! database instance to store regular expressions
    };
}

#endif // REGEXDATABASE_H
