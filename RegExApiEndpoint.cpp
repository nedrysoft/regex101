#include "RegExApiEndpoint.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>

constexpr auto permalinkStringLength=6;
constexpr auto deleteCodeStringLength=24;

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

    return QVariant::Map;
}

QVariant Nedrysoft::RegExApiEndpoint::localStorageGetItem(const QVariant &key)
{
    if (m_settings->contains(key.toString())) {
        return(m_settings->value(key.toString()));
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

    if (method=="POST") {
        return(processPostRequest(pathParameter, requestParameter));
    } else if (method=="DELETE") {
        return(processDeleteRequest(pathParameter, requestParameter));
    }

    return(processGetRequest(pathParameter, requestParameter));
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
            regexEditor["regexVersions"] = 0;
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

QVariant Nedrysoft::RegExApiEndpoint::processGetRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    QJsonObject jsonResponse;
    auto path = pathParameter.toString();

    auto staticMatch = QRegularExpression(R"(^\/static\/?)").match(path);
    auto detailsMatch = QRegularExpression(R"(\/api\/library\/details\/(?P<permalinkFragment>.*))").match(path);
    auto libraryMatch = QRegularExpression(R"(\/api\/library\/\d*\/\?orderBy=(?P<orderBy>.*)\&search=(?P<search>.*))").match(path);

    if (staticMatch.hasMatch()) {
        return processGetStatic(pathParameter, requestParameter, staticMatch);
    } else  if (detailsMatch.hasMatch()) {
        return processGetItemDetails(pathParameter, requestParameter, detailsMatch);
    } else if (libraryMatch.hasMatch()) {
        return processGetLibraryItems(pathParameter, requestParameter, libraryMatch);
    }

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processPostRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    Q_UNUSED(requestParameter);

    QJsonObject jsonResponse;
    auto path = pathParameter.toString();

    if (QRegularExpression(R"(^\/api\/regex\/?)").match(path).hasMatch()) {
        return(processSaveRequest(pathParameter, requestParameter));
    } else if (QRegularExpression(R"(^\/api\/regex\/fork)").match(path).hasMatch()) {
        return(processForkRequest(pathParameter, requestParameter));
    }

    return QVariantMap();
}

QVariant Nedrysoft::RegExApiEndpoint::processDeleteRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);

    QJsonObject jsonResponse;

    jsonResponse["message"] = "Your regex has been deleted.";

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processSaveRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);

    QJsonObject jsonResponse;
    QJsonObject bodyObject = QJsonDocument::fromJson(requestParameter.toMap()["body"].toByteArray()).object();
    QString permalinkFragment = createPermalinkFragment();
    QString deleteCode = createDeleteCode();

    QSqlQuery query;

    query.prepare("INSERT INTO expressions (regex, testString, flags, delimeter, flavor, permalinkFragment, upvotes, downvotes, userVote, version, dateModified, title, description) \
                   VALUES (:regex, :testString, :flags, :delimeter, :flavor, :permalinkFragment, :upvotes, :downvotes, :userVote, :version, :dateModified, :title, :description)"); //, unitTests

    query.bindValue(":regex", bodyObject["regex"].toString());
    query.bindValue(":testString", bodyObject["testString"].toString());
    query.bindValue(":flags", bodyObject["flags"].toString());
    query.bindValue(":delimeter", bodyObject["delimiter"].toString());
    query.bindValue(":flavor", bodyObject["flavor"].toString());
    //query.bindValue(":unitTests", bodyObject["unitTests"].toVariant());
    query.bindValue(":permalinkFragment", permalinkFragment);
    query.bindValue(":deleteCode", deleteCode);
    query.bindValue(":upvote", 0);
    query.bindValue(":downvote", 0);
    query.bindValue(":userVote", 0);
    query.bindValue(":version", 1);
    query.bindValue(":dateModified", QDateTime::currentDateTime().toTime_t());

    /*qDebug() << query.exec();

    qDebug() << query.lastError().text();*/

    jsonResponse["deleteCode"] = deleteCode;
    jsonResponse["permalinkFragment"] = permalinkFragment;
    jsonResponse["version"] = 1;
    jsonResponse["isLibraryEntry"] = false;

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processForkRequest(const QVariant &pathParameter, const QVariant &requestParameter) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);

    QJsonObject jsonResponse;

    jsonResponse["deleteCode"] = "NktBfSnFCsT2M5KbWQlatnqS";
    jsonResponse["permalinkFragment"] = "YOMAMA";
    jsonResponse["version"] = 1;
    jsonResponse["isLibraryEntry"] = false;

    return QJsonDocument(jsonResponse).toJson();
}

QVariant Nedrysoft::RegExApiEndpoint::processGetLibraryItems(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);
    Q_UNUSED(match);

    QJsonArray resultEntries;
    QJsonObject result;
    QJsonObject jsonResponse;

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

QVariant Nedrysoft::RegExApiEndpoint::processGetItemDetails(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);
    Q_UNUSED(match);

    QJsonObject jsonResponse;
    QSqlQuery query;
    auto path = pathParameter.toString();

    query.prepare("SELECT * FROM expressions WHERE permalinkFragment=:permalinkFragment");
    query.bindValue(":permalinkFragment", match.captured("permalinkFragment"));

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

QVariant Nedrysoft::RegExApiEndpoint::processGetStatic(const QVariant &pathParameter, const QVariant &requestParameter, const QRegularExpressionMatch &match) const
{
    Q_UNUSED(pathParameter);
    Q_UNUSED(requestParameter);
    Q_UNUSED(match);

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
    return(createRandomString(permalinkStringLength));
}

QString Nedrysoft::RegExApiEndpoint::createDeleteCode() const
{
    return(createRandomString(deleteCodeStringLength));
}

