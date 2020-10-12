#include "RegExDatabase.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QStandardPaths>

Nedrysoft::RegExDatabase::RegExDatabase()
{
    QFileInfo fileInfo(QDir::cleanPath(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)+QDir::separator()+"regex101.sqlite"));
    bool fileExists = fileInfo.exists();

    m_database = QSqlDatabase::addDatabase("QSQLITE");

    m_database.setDatabaseName(fileInfo.absoluteFilePath());

    m_database.open();

    if (!fileExists) {
        initialiseDatabase();
    }
}

Nedrysoft::RegExDatabase *Nedrysoft::RegExDatabase::getInstance()
{
    static Nedrysoft::RegExDatabase instance;

    return(&instance);
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
