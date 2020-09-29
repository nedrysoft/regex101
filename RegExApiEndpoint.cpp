#include "RegExApiEndpoint.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>

Nedrysoft::RegExApiEndpoint::RegExApiEndpoint()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");

    m_database.setDatabaseName(QDir::cleanPath(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)+QDir::separator()+"regex101.sqlite"));

    m_database.open();
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

    QVariantMap responseMap;

    if (method=="") {
        if (QRegularExpression(R"(^\/static\/?)").match(path).hasMatch()) {
            QFile file(QDir::cleanPath(":/regex101/"+path));

            if (file.open(QFile::ReadOnly)) {
               return file.readAll();
            }
        }
    } else if (method=="POST") {
        if (QRegularExpression(R"(^\/api\/regex\/?)").match(path).hasMatch()) {
            responseMap["deleteCode"] = "NktBHSnFCsT2M5KbWQlatnqS";
            responseMap["permalinkFragment"] = "B123";
            responseMap["version"] = 1;
            responseMap["isLibraryEntry"] = false;
        } else if (QRegularExpression(R"(^\/api\/regex\/fork)").match(path).hasMatch()) {
            responseMap["deleteCode"] = "NktBfSnFCsT2M5KbWQlatnqS";
            responseMap["permalinkFragment"] = "YOMAMA";
            responseMap["version"] = 1;
            responseMap["isLibraryEntry"] = false;
        }

        return QJsonDocument(QJsonObject::fromVariantMap(responseMap)).toJson();
    } else if (method=="DELETE") {
        responseMap["message"] = "Your regex has been deleted.";

        return QJsonDocument(QJsonObject::fromVariantMap(responseMap)).toJson();
    } else if (method=="GET") {
        if (QRegularExpression(R"(^\/static\/)").match(path).hasMatch()) {
            QFile file(QDir::cleanPath(":/regex101/"+path));

            if (file.open(QFile::ReadOnly)) {
               return file.readAll();
            }
        } else {
            auto match = QRegularExpression(R"(\/api\/library\/\d*\/\?orderBy=(?P<orderBy>.*)\&search=(?P<search>.*))").match(path);

            if (match.hasMatch()) {
                QVariantMap entryMap;
                QVariantList entries;
                QVariantMap resultMap;

                QSqlQuery query("SELECT * FROM expressions");

                if (query.exec()) {
                    if (query.first()) {
                        do {
                            entryMap["title"] = query.value("title").toString();
                            entryMap["description"] = query.value("description").toString();
                            entryMap["dateModified"] = QDateTime::fromTime_t(query.value("dateModified").toInt());
                            entryMap["author"] = query.value("author").toString();
                            entryMap["flavor"] = query.value("flavor").toString();
                            entryMap["version"] = query.value("version").toInt();
                            entryMap["permalinkFragment"] = query.value("permalinkFragment").toString();
                            entryMap["upvotes"] = query.value("upvotes").toInt();
                            entryMap["downvotes"] = query.value("downvotes").toInt();
                            entryMap["regex"] = query.value("regex").toInt();
                            entryMap["userVote"] = QVariant();

                            entries.append(entryMap);
                        } while(query.next());
                    }
                }

                resultMap["data"] = entries;

                return QJsonDocument(QJsonObject::fromVariantMap(resultMap)).toJson();
            }
        }
    }

    return(QVariant("{}"));
}
