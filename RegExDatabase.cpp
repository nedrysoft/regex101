#include "MainWindow.h"
#include "RegExDatabase.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

constexpr auto defaultDatabaseType = "QSQLITE";

Nedrysoft::RegExDatabase::RegExDatabase()
{
    QVariantMap settingsMap;

    auto defaultDatabaseFile = QDir::cleanPath(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)+QDir::separator()+"regex101.sqlite");

    settingsMap = getSettings();

    auto databaseDriver = settingsMap["databaseDriver"].toString();
    auto databaseName = settingsMap["databaseName"].toString();
    auto databaseHost = settingsMap["databaseHost"].toString();;
    auto databaseUser = settingsMap["databaseUser"].toString();
    auto databasePassword = settingsMap["databasePassword"].toString();

    m_database = QSqlDatabase::addDatabase(databaseDriver);

    m_database.setDatabaseName(databaseName);
    m_database.setHostName(databaseHost);
    m_database.setUserName(databaseUser);
    m_database.setPassword(databasePassword);

    if (!m_database.open()) {
        auto errorTitle = QCoreApplication::translate("database", "Unable to open error");
        auto errorMessage = QCoreApplication::translate("database", "Unable to open database, the following error was reported.\r\n\r\n%1").arg(m_database.lastError().text());;

        QMessageBox::warning(Nedrysoft::MainWindow::getInstance(), errorTitle, errorMessage);
    } else {
        initialiseDatabase();
    }
}

Nedrysoft::RegExDatabase *Nedrysoft::RegExDatabase::getInstance()
{
    static Nedrysoft::RegExDatabase instance;

    return(&instance);
}

QVariantMap Nedrysoft::RegExDatabase::getSettings()
{
    QSettings settings;
    auto defaultDatabaseFile = QDir::cleanPath(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)+QDir::separator()+"regex101.sqlite");

    QVariantMap settingsMap;

    settingsMap["databaseDriver"] = settings.value(QStringLiteral("databaseDriver"), defaultDatabaseType);
    settingsMap["databaseHost"] = settings.value(QStringLiteral("databaseHost"), QStringLiteral(""));
    settingsMap["databaseName"] = settings.value(QStringLiteral("databaseName"), defaultDatabaseFile);
    settingsMap["databaseUser"] = settings.value(QStringLiteral("databaseUser"), QStringLiteral(""));
    settingsMap["databasePassword"]= settings.value(QStringLiteral("databasePassword"), QStringLiteral(""));

    return settingsMap;
}

void Nedrysoft::RegExDatabase::storeSettings(QVariantMap settingsMap)
{
    QSettings settings;

    settings.setValue("databaseDriver", settingsMap["databaseDriver"]);
    settings.setValue("databaseHost", settingsMap["databaseHost"]);
    settings.setValue("databaseName", settingsMap["databaseName"]);
    settings.setValue("databaseUser", settingsMap["databaseUser"]);
    settings.setValue("databasePassword", settingsMap["databasePassword"]);
}

QSqlQuery Nedrysoft::RegExDatabase::prepareQuery(QString queryName)
{
    QFile sqlFile(QString(":/regex101/sql/%1.sql").arg(queryName));
    QSqlQuery query;

    if (sqlFile.open(QFile::ReadOnly)) {
        query.prepare(QString::fromLatin1(sqlFile.readAll()));
    }

    return query;
}

bool Nedrysoft::RegExDatabase::initialiseDatabase()
{
    QFile sqlFile(QString(":/regex101/sql/createDatabase.sql"));
    QString sqlStatement;
    QStringList commandList;
    QSqlQuery query;
    bool returnValue = true;
    char characterIn;

    if (sqlFile.open(QFile::ReadOnly)) {
        while(sqlFile.getChar(&characterIn)) {
            if ((characterIn=='\r') || (characterIn=='\n'))
                continue;

            sqlStatement = sqlStatement.append(characterIn);

            if (characterIn==';') {
                commandList.append(sqlStatement);

                sqlStatement.clear();
            }
        }
    }

    for(auto sqlStatement : commandList) {
        query.prepare(sqlStatement);

        if (!query.exec(sqlStatement)) {
            returnValue = false;
        }
    }

    return commandList.count()?returnValue:false;
}
