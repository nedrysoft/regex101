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

#include "RegExApiEndpoint.h"
#include "RegExDatabase.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>

constexpr auto permalinkStringLength=6;
constexpr auto deleteCodeStringLength=24;

Nedrysoft::RegExApiEndpoint::RegExApiEndpoint()
{
    m_settings = new QSettings("nedrysoft", "regex101");

    m_database = RegExDatabase::getInstance();

    m_librarySearchSortMap[""] = "dateModified DESC";
    m_librarySearchSortMap["MOST_RECENT"] = "dateModified DESC";
    m_librarySearchSortMap["MOST_POINTS"] = "userVote DESC";
    m_librarySearchSortMap["LEAST_POINTS"] = "userVote ASC";
    m_librarySearchSortMap["RELEVANCE"] = "upVotes";
}

Nedrysoft::RegExApiEndpoint *Nedrysoft::RegExApiEndpoint::getInstance()
{
    static RegExApiEndpoint *instance = new RegExApiEndpoint();

    return instance;
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageSetItem(const QVariant &key, const QVariant &value)
{
    m_settings->setValue(key.toString(), value);

    return QVariant::Map;
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageGetItem(const QVariant &key)
{
    if (m_settings->contains(key.toString())) {
        return m_settings->value(key.toString());
    } else {
        return QVariant::Map;
    }
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageRemoveItem(const QVariant &key)
{
    m_settings->remove(key.toString());

    return QVariant::Map;
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageClear()
{
    m_settings->clear();

    return QVariant::Map;
}

QVariant Nedrysoft::RegExApiEndpoint::fetch(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    auto valueMap = requestParameter.toMap();
    auto method = valueMap["method"].toString();

    if (method=="PUT") {
        return processPutRequest(pathParameter, requestParameter);
    } else if (method=="POST") {
        return processPostRequest(pathParameter, requestParameter);
    } else if (method=="DELETE") {
        return processDeleteRequest(pathParameter, requestParameter);
    }

    return processGetRequest(pathParameter, requestParameter);
}

bool Nedrysoft::RegExApiEndpoint::regex(QJsonObject &stateObject, QString permalinkFragment, int version)
{
    QSqlQuery query;
    QJsonObject general, matchResult, regexEditor, resultMap, libraryEntry;
    bool didLoadRegEx = false;

    query = m_database->prepareQuery("getStoredExpression");

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
            regexEditor["regexVersions"] = query.value("versionCount").toInt();
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

    return didLoadRegEx;
}

QVariant Nedrysoft::RegExApiEndpoint::processGetRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    auto path = pathParameter.toString();

    auto staticMatch = QRegularExpression(R"(^\/static\/?)").match(path);
    auto detailsMatch = QRegularExpression(R"(\/api\/library\/details\/(?P<permalinkFragment>.*))").match(path);
    auto libraryMatch = QRegularExpression(R"(\/api\/library\/\d*\/\?orderBy=(?P<orderBy>.*)\&search=(?P<search>.*))").match(path);
    auto regexMatch = QRegularExpression(R"(\/api\/regex\/(?P<permalinkFragment>.*))").match(path);

    if (staticMatch.hasMatch()) {
        return processGetStatic(pathParameter, requestParameter, staticMatch);
    } else  if (detailsMatch.hasMatch()) {
        return processGetItemDetails(pathParameter, requestParameter, detailsMatch);
    } else if (libraryMatch.hasMatch()) {
        return processGetLibraryItems(pathParameter, requestParameter, libraryMatch);
    } else if (regexMatch.hasMatch()) {
        return processGetRegEx(pathParameter, requestParameter, libraryMatch);
    }

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processPutRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    auto path = pathParameter.toString();

    auto tagsMatch = QRegularExpression(R"(\/api\/user\/history\/(?P<permalinkFragment>[A-Z|a-z|0-9]*)\/(?P<version>\d*)\/(?P<action>([a-z|A-Z|0-9]*))$)").match(path);

    if (tagsMatch.hasMatch()) {
        return processPutHistoryRequest(pathParameter, requestParameter, tagsMatch);
    }

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processPostRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    auto path = pathParameter.toString();

    qDebug() << path;

    if (QRegularExpression(R"(^\/api\/regex\/fork)").match(path).hasMatch()) {
            return processForkRequest(pathParameter, requestParameter);
    } else  if (QRegularExpression(R"(^\/api\/regex\/?)").match(path).hasMatch()) {
        return processSaveRequest(pathParameter, requestParameter);
    }

    return QVariantMap();
}

QVariant Nedrysoft::RegExApiEndpoint::processDeleteRequest([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;

    jsonResponse["message"] = "Your regex has been deleted.";

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processSaveRequest([[maybe_unused]] const QVariant &pathParameter, const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    QJsonObject bodyObject = QJsonDocument::fromJson(requestParameter.toMap()["body"].toByteArray()).object();
    QString permalinkFragment = createPermalinkFragment();
    QString deleteCode = createDeleteCode();

    QSqlQuery query;

    if (bodyObject["permalinkFragment"].isNull())
    {
        query = m_database->prepareQuery("insertExpression");

        query.bindValue(":permalinkFragment", permalinkFragment);
        query.bindValue(":deleteCode", deleteCode);
        query.bindValue(":upvote", 0);
        query.bindValue(":downvote", 0);
        query.bindValue(":userVote", 0);
        query.bindValue(":dateModified", QDateTime::currentDateTime().toTime_t());

        if (!query.exec()) {
            // return some error
        }

        query = m_database->prepareQuery("insertFirstVersion");

        query.bindValue(":permalinkFragment", permalinkFragment);
        query.bindValue(":regex", bodyObject["regex"].toString());
        query.bindValue(":testString", bodyObject["testString"].toString());
        query.bindValue(":flags", bodyObject["flags"].toString());
        query.bindValue(":delimeter", bodyObject["delimiter"].toString());
        query.bindValue(":flavor", bodyObject["flavor"].toString());
        query.bindValue(":substitution", bodyObject["substitution"].toString());
        query.bindValue(":version", 1);

        if (query.exec()) {
            jsonResponse["deleteCode"] = deleteCode;
            jsonResponse["permalinkFragment"] = permalinkFragment;
            jsonResponse["version"] = 1;
            jsonResponse["isLibraryEntry"] = false;
        } else {
            // SQL insert failed
        }
    } else {
        query = m_database->prepareQuery("insertNewVersion");

        query.bindValue(":permalinkFragment", bodyObject["permalinkFragment"].toString());
        query.bindValue(":regex", bodyObject["regex"].toString());
        query.bindValue(":testString", bodyObject["testString"].toString());
        query.bindValue(":flags", bodyObject["flags"].toString());
        query.bindValue(":delimeter", bodyObject["delimiter"].toString());
        query.bindValue(":flavor", bodyObject["flavor"].toString());
        query.bindValue(":substitution", bodyObject["substitution"].toString());

        if (query.exec()) {
            jsonResponse["deleteCode"] = deleteCode;
            jsonResponse["permalinkFragment"] = permalinkFragment;
            jsonResponse["version"] = 1;
            jsonResponse["isLibraryEntry"] = false;
        } else {
            // SQL insert failed
        }
    }

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processForkRequest([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    QJsonObject bodyObject = QJsonDocument::fromJson(requestParameter.toMap()["body"].toByteArray()).object();
    QString permalinkFragment = createPermalinkFragment();
    QString deleteCode = createDeleteCode();

    auto query = m_database->prepareQuery("insertExpression");

    query.bindValue(":permalinkFragment", permalinkFragment);
    query.bindValue(":deleteCode", deleteCode);
    query.bindValue(":upvote", 0);
    query.bindValue(":downvote", 0);
    query.bindValue(":userVote", 0);
    query.bindValue(":dateModified", QDateTime::currentDateTime().toTime_t());

    if (!query.exec()) {
        // return some error
    }

    query = m_database->prepareQuery("insertFirstVersion");

    query.bindValue(":permalinkFragment", permalinkFragment);
    query.bindValue(":regex", bodyObject["regex"].toString());
    query.bindValue(":testString", bodyObject["testString"].toString());
    query.bindValue(":flags", bodyObject["flags"].toString());
    query.bindValue(":delimeter", bodyObject["delimiter"].toString());
    query.bindValue(":flavor", bodyObject["flavor"].toString());
    query.bindValue(":substitution", bodyObject["substitution"].toString());
    query.bindValue(":version", 1);

    if (query.exec()) {
        jsonResponse["deleteCode"] = deleteCode;
        jsonResponse["permalinkFragment"] = permalinkFragment;
        jsonResponse["version"] = 1;
        jsonResponse["isLibraryEntry"] = false;
    } else {
        // SQL insert failed
    }

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processGetLibraryItems([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, [[maybe_unused]] const QRegularExpressionMatch &match) const
{
    QJsonArray jsonResultArray;
    QJsonObject jsonResponse, jsonResult;

    QSqlQuery query = m_database->prepareQuery("getLibraryItems");

    query.bindValue(":search", QString("%%1%").arg(match.captured("search")));
    query.bindValue(":orderBy", m_librarySearchSortMap[match.captured("orderBy")]);

    if (query.exec()) {
        if (query.first()) {
            do {
                jsonResult["title"] = query.value("title").toString();
                jsonResult["description"] = query.value("description").toString();
                jsonResult["dateModified"] = query.value("dateModified").toInt();
                jsonResult["author"] = query.value("author").toString();
                jsonResult["flavor"] = query.value("flavor").toString();
                jsonResult["version"] = query.value("version").toInt();
                jsonResult["permalinkFragment"] = query.value("permalinkFragment").toString();
                jsonResult["upvotes"] = query.value("upvotes").toInt();
                jsonResult["downvotes"] = query.value("downvotes").toInt();
                jsonResult["regex"] = query.value("regex").toString();
                jsonResult["userVote"] = QJsonValue::Null;

                jsonResultArray.append(jsonResult);
            } while(query.next());
        }
    }

    jsonResponse["data"] = jsonResultArray;

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processGetItemDetails([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    QJsonObject jsonResponse;
    QSqlQuery query;
    auto path = pathParameter.toString();

    query = m_database->prepareQuery("getItemDetails");

    query.bindValue(":permalinkFragment", match.captured("permalinkFragment"));

    if (query.exec()) {
        if (query.first()) {
            jsonResponse["title"] = query.value("title").toString();
            jsonResponse["description"] = query.value("description").toString();
            jsonResponse["dateModified"] = QDateTime::fromTime_t(query.value("dateModified").toInt()).toString();
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

QVariant Nedrysoft::RegExApiEndpoint::processGetStatic([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, [[maybe_unused]] const QRegularExpressionMatch &match) const
{
    auto path = pathParameter.toString();

    QFile file(QDir::cleanPath(":/regex101/"+path));

    if (file.open(QFile::ReadOnly)) {
       return file.readAll();
    }

    return QString();
}

QString Nedrysoft::RegExApiEndpoint::createRandomString(int numberOfCharacters) const
{
    QVector<char> characterMap;
    QString permalinkFragment;

    for (char currentCharacter='A';currentCharacter<='Z';currentCharacter++) {
        characterMap.append(currentCharacter);
        characterMap.append(tolower(currentCharacter));
    }

    for (char currentCharacter='0';currentCharacter<='9';currentCharacter++) {
        characterMap.append(currentCharacter);
    }

    for (int currentCharacter=0;currentCharacter<numberOfCharacters;currentCharacter++) {
        permalinkFragment = permalinkFragment.append(characterMap.at(QRandomGenerator::global()->bounded(characterMap.count())));
    }

    return permalinkFragment;
}

QString Nedrysoft::RegExApiEndpoint::createPermalinkFragment() const
{
    return createRandomString(permalinkStringLength);
}

QString Nedrysoft::RegExApiEndpoint::createDeleteCode() const
{
    return createRandomString(deleteCodeStringLength);
}

QVariant Nedrysoft::RegExApiEndpoint::processPutHistoryRequest([[maybe_unused]] const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    QSqlQuery query;
    auto bodyObject = QJsonDocument::fromJson(requestParameter.toMap()["body"].toByteArray()).object();
    auto permalinkFragment = match.captured("permalinkFragment");
    auto version = match.captured("version");
    auto action = match.captured("action");

    if (action=="tags") {
        auto tags = bodyObject["tags"].toArray();

        query = m_database->prepareQuery("deleteTags");

        query.bindValue(":permalinkFragment", permalinkFragment);

        if (query.exec()) {
            for(auto tag: tags) {
                query = m_database->prepareQuery("insertTag");

                query.bindValue(":tag", tag.toString());
                query.bindValue(":permalinkFragment", permalinkFragment);

                if (query.exec()) {
                    // success
                }
            }
        }
    } else if (action=="title") {
        query = m_database->prepareQuery("updateTitle");

        query.bindValue(":title", bodyObject["title"].toString());
        query.bindValue(":permalinkFragment", permalinkFragment);

        if (query.exec()) {
            // success
        }
    }

    return QString();
}

void Nedrysoft::RegExApiEndpoint::notifyApplication([[maybe_unused]] QVariant message) const
{
}

QVariant Nedrysoft::RegExApiEndpoint::processGetRegEx([[maybe_unused]] const QVariant &pathParameter, [[maybe_unused]] const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    QSqlQuery query;
    QJsonObject jsonResponse;

    query = m_database->prepareQuery("getStoredExpression");

    query.bindValue(":permalinkFragment", match.captured("permalinkFragment"));
    query.bindValue(":version", 1);

    if (query.exec()) {
        if (query.first()) {
            jsonResponse["permalinkFragment"] =  match.captured("permalinkFragment");
            jsonResponse["versions"] =  query.value("versionCount").toInt();
        }
    }

    return QJsonDocument(jsonResponse).toJson();
}
