#include "RegExApiEndpoint.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Nedrysoft::RegExApiEndpoint::RegExApiEndpoint()
{
}

QString Nedrysoft::RegExApiEndpoint::fetch(const QVariant &pathParameter, const QVariant &requestParameter) const
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

    if (method=="POST") {
        responseMap["deleteCode"] = "NktBfSnFCsT2M5KbWQlatnqS";
        responseMap["permalinkFragment"] = "YOMAMA";
        responseMap["version"] = 1;
        responseMap["isLibraryEntry"] = false;

        return QJsonDocument(QJsonObject::fromVariantMap(responseMap)).toJson();
    } else if (method=="DELETE") {
        responseMap["message"] = "Your regex has been deleted.";

        return QJsonDocument(QJsonObject::fromVariantMap(responseMap)).toJson();
    }

    return("{}");
}
