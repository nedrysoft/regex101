#include "RegExApiEndpoint.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>

Nedrysoft::RegExApiEndpoint::RegExApiEndpoint()
{
    m_settings = new QSettings("nedrysoft", "regex101");

    m_database = QSqlDatabase::addDatabase("QSQLITE");

    m_database.setDatabaseName(QDir::cleanPath(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)+QDir::separator()+"regex101.sqlite"));

    m_database.open();
}

Nedrysoft::RegExApiEndpoint *Nedrysoft::RegExApiEndpoint::getInstance()
{
    static RegExApiEndpoint *instance = new RegExApiEndpoint();

    return instance;
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageSetItem(const QVariant &key, const QVariant &value)
{
    m_settings->setValue(key.toString(), value);

    return QVariant();
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageGetItem(const QVariant &key)
{
    if (m_settings->contains(key.toString())) {
        return(m_settings->value(key.toString()).toString());
    } else {
        return QVariant();
    }
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageRemoveItem(const QVariant &key)
{
    m_settings->remove(key.toString());

    return QVariant();
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageClear()
{
    m_settings->clear();

    return QVariant();
}

QVariant Nedrysoft::RegExApiEndpoint::fetch(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    auto valueMap = requestParameter.toMap();
    auto path = pathParameter.toString();

    auto method = valueMap["method"].toString();
    auto body = valueMap["body"].toString();
    auto credentials = valueMap["credentials"].toString();
    auto headers = valueMap["headers"].toMap();

    /*qDebug() << "path:" << path;
    qDebug() << "method: " << method;
    qDebug() << "headers: " << headers;
    qDebug() << "body:" << body;*/

    QJsonObject jsonResponse;

    if (method=="") {
        if (QRegularExpression(R"(^\/static\/?)").match(path).hasMatch()) {
            QFile file(QDir::cleanPath(":/regex101/"+path));

            if (file.open(QFile::ReadOnly)) {
               return file.readAll();
            }
        }
    } else if (method=="POST") {
        if (QRegularExpression(R"(^\/api\/regex\/?)").match(path).hasMatch()) {
            jsonResponse["deleteCode"] = "NktBHSnFCsT2M5KbWQlatnqS";
            jsonResponse["permalinkFragment"] = "B123";
            jsonResponse["version"] = 1;
            jsonResponse["isLibraryEntry"] = false;
        } else if (QRegularExpression(R"(^\/api\/regex\/fork)").match(path).hasMatch()) {
            jsonResponse["deleteCode"] = "NktBfSnFCsT2M5KbWQlatnqS";
            jsonResponse["permalinkFragment"] = "YOMAMA";
            jsonResponse["version"] = 1;
            jsonResponse["isLibraryEntry"] = false;
        }

        return QJsonDocument(jsonResponse).toJson();
    } else if (method=="DELETE") {
        jsonResponse["message"] = "Your regex has been deleted.";

        return QJsonDocument(jsonResponse).toJson();
    } else if (method=="GET") {
        if (QRegularExpression(R"(^\/static\/)").match(path).hasMatch()) {
            QFile file(QDir::cleanPath(":/regex101/"+path));

            if (file.open(QFile::ReadOnly)) {
               return file.readAll();
            }
        } else {
            auto detailsMatch = QRegularExpression(R"(\/api\/library\/details\/(?P<permalinkFragment>.*))").match(path);

            if (detailsMatch.hasMatch()) {
                QJsonObject jsonResponse;
                QSqlQuery query;

                query.prepare("SELECT * FROM expressions WHERE permalinkFragment=:permalinkFragment");
                query.bindValue(":permalinkFragment", detailsMatch.captured("permalinkFragment"));

                if (query.exec()) {
                    if (query.first()) {
                        jsonResponse["title"] = query.value("title").toString();
                        jsonResponse["description"] = query.value("description").toString();
                        jsonResponse["dateModified"] = query.value("dateModified").toInt();
                        jsonResponse["author"] = query.value("author").toString();
                        jsonResponse["flavor"] = query.value("flavor").toString();
                        jsonResponse["regex"] = query.value("regex").toString();
                        jsonResponse["delimeter"] = query.value("delimeter").toString();
                        jsonResponse["version"] = query.value("version").toInt();
                        jsonResponse["permalinkFragment"] = query.value("permalinkFragment").toString();
                        jsonResponse["flags"] = query.value("flags").toString();
                    }
                }

                return QJsonDocument(jsonResponse).toJson();
            }

            auto libraryMatch = QRegularExpression(R"(\/api\/library\/\d*\/\?orderBy=(?P<orderBy>.*)\&search=(?P<search>.*))").match(path);

            if (libraryMatch.hasMatch()) {
                QJsonObject jsonResponse;
                QJsonArray resultEntries;
                QJsonObject result;

                QSqlQuery query("SELECT * FROM expressions");

                if (query.exec()) {
                    if (query.first()) {
                        do {
                            result["title"] = query.value("title").toString();
                            result["description"] = query.value("description").toString();
                            result["dateModified"] = query.value("dateModified").toInt();
                            result["author"] = query.value("author").toString();
                            result["flavor"] = query.value("flavor").toString();
                            result["version"] = query.value("version").toInt();
                            result["permalinkFragment"] = query.value("permalinkFragment").toString();
                            result["upvotes"] = query.value("upvotes").toInt();
                            result["downvotes"] = query.value("downvotes").toInt();
                            result["regex"] = query.value("regex").toString();
                            result["userVote"] = QJsonValue::Null;

                            resultEntries.append(result);
                        } while(query.next());
                    }
                }

                jsonResponse["data"] = resultEntries;

                return QJsonDocument(jsonResponse).toJson();
            }
        }
    }

    return QVariant("{}");
}

bool Nedrysoft::RegExApiEndpoint::regex(QJsonObject &stateObject, QString permalinkFragment, int version)
{
    QSqlQuery query;
    QJsonObject general, matchResult, regexEditor, resultMap, libraryEntry;
    bool didLoadRegEx = false;

    query.prepare("SELECT * FROM expressions WHERE permalinkFragment=:permalinkFragment AND version=:version");
    query.bindValue(":permalinkFragment", permalinkFragment);
    query.bindValue(":version", version);

    matchResult["data"] = QJsonArray();
    matchResult["time"] = 0;

    if (stateObject.contains("general")) {
        general = stateObject["general"].toObject();
    }

    if (stateObject.contains("regexEditor")) {
        regexEditor = stateObject["regexEditor"].toObject();
    }

    if (stateObject.contains("libraryEntry")) {
        libraryEntry = stateObject["libraryEntry"].toObject();
    }

    if (query.exec()) {
        if (query.first()) {
            regexEditor["title"] = query.value("title").toString();
            regexEditor["description"] = query.value("description").toString();
            regexEditor["dateModified"] = query.value("dateModified").toInt();
            regexEditor["author"] = query.value("author").toString();
            regexEditor["flavor"] = query.value("flavor").toString();
            regexEditor["regex"] = query.value("regex").toString();
            regexEditor["delimeter"] = query.value("delimeter").toString();
            regexEditor["version"] = query.value("version").toInt();
            regexEditor["permalinkFragment"] = query.value("permalinkFragment").toString();
            regexEditor["flags"] = query.value("flags").toString();
            regexEditor["testString"] = query.value("testString").toString();
            regexEditor["matchResult"] = matchResult;
            regexEditor["error"] = query.value("error").toString();
            regexEditor["substString"] = query.value("substString").toString();
            regexEditor["hasUnsavedData"] = false;
            regexEditor["regexVersions"] = 1;
            regexEditor["showMatchArea"] = false;
            regexEditor["showSubstitutionArea"] = true;
            regexEditor["showUnitTestArea"] = false;

            general["permalinkFragment"] = permalinkFragment;
            general["version"] = query.value("version").toInt();

            libraryEntry["title"] = query.value("title").toString();
            libraryEntry["description"] = query.value("description").toString();
            libraryEntry["author"] = query.value("author").toString();

            didLoadRegEx = true;
        }
    }

    stateObject["regexEditor"] = regexEditor;
    stateObject["general"] = general;
    stateObject["libraryEntry"] = libraryEntry;

    return(didLoadRegEx);
}
