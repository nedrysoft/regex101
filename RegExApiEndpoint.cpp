#include "RegExApiEndpoint.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Nedrysoft::RegExApiEndpoint::RegExApiEndpoint()
{
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

                entryMap["title"] = "Real Numbers";
                entryMap["description"] = "";
                entryMap["dateModified"] = "2020-09-28T18:47:07.000Z";
                entryMap["author"] = "";
                entryMap["flavor"] = "pcre";
                entryMap["version"] = 1;
                entryMap["permalinkFragment"] = "yNU0pf";
                entryMap["upvotes"] = 0;
                entryMap["downvotes"] = 0;
                entryMap["userVote"] = QVariant();

                entries.append(entryMap);

                resultMap["data"] = entries;

                return QJsonDocument(QJsonObject::fromVariantMap(resultMap)).toJson();
            }
        }
    }

    return(QVariant("{}"));
}
