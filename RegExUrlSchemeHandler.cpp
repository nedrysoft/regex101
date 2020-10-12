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
#include "RegExUrlSchemeHandler.h"

#include <QBuffer>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QUrlQuery>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>

constexpr auto advertResponse = R"(({"ads":[{}]});)";
constexpr auto htmlMimeType = "text/html";
constexpr auto javascriptMimeType = "application/javascript";
constexpr auto qrcRootFolder = ":/";
constexpr auto javascriptBridgeFilename = ":/regex101/regexbridge.js";

auto GET(QByteArrayLiteral("GET"));
auto POST(QByteArrayLiteral("POST"));
auto DELETE(QByteArrayLiteral("DELETE"));

QString Nedrysoft::RegExUrlSchemeHandler::name()
{
    return QStringLiteral("regex101");
}

QString Nedrysoft::RegExUrlSchemeHandler::scheme()
{
    return name()+QStringLiteral(":");
}

QString Nedrysoft::RegExUrlSchemeHandler::root()
{
    return scheme()+QStringLiteral("/");
}

void Nedrysoft::RegExUrlSchemeHandler::registerScheme()
{
    QWebEngineUrlScheme scheme((RegExUrlSchemeHandler::name().toUtf8()));

    scheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                    QWebEngineUrlScheme::LocalScheme |
                    QWebEngineUrlScheme::LocalAccessAllowed |
                    QWebEngineUrlScheme::ContentSecurityPolicyIgnored |
                    QWebEngineUrlScheme::ServiceWorkersAllowed |
                    QWebEngineUrlScheme::CorsEnabled |
                    QWebEngineUrlScheme::ContentSecurityPolicyIgnored);

    QWebEngineUrlScheme::registerScheme(scheme);
}

Nedrysoft::RegExUrlSchemeHandler::RegExUrlSchemeHandler(QString resourceRootFolder)
{
    m_resourceRootFolder = resourceRootFolder;

    QFile regexBridgeFile(javascriptBridgeFilename);

    if (regexBridgeFile.open(QFile::ReadOnly)) {
        m_injectedJavascript = QString::fromLatin1(regexBridgeFile.readAll());
    }
}

void Nedrysoft::RegExUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    auto method = job->requestMethod();
    auto url = job->requestUrl().path(QUrl::FormattingOptions(QUrl::StripTrailingSlash));
    QMimeDatabase db;

    if (method==GET) {
        auto resourceUrl = job->requestUrl();

        resourceUrl.setScheme(qrcRootFolder);
        resourceUrl.setPath(m_resourceRootFolder+resourceUrl.path());

        // if request is for the advert endpoint then send a dummy json object

        if (QRegularExpression(R"(\/ads\/.*\.json)").match(job->requestUrl().path()).hasMatch()) {
            auto urlQuery = QUrlQuery(job->requestUrl());
            auto jsonResponseString = QString("%1%2").arg(urlQuery.queryItemValue("callback")).arg(advertResponse);
            auto *buffer = new QBuffer(job);

            buffer->open(QIODevice::WriteOnly);
            buffer->write(jsonResponseString.toUtf8());
            buffer->close();

            connect(job, &QObject::destroyed, buffer, &QObject::deleteLater);

            job->reply(javascriptMimeType, buffer);

            return;
        }

        // if there was no file in the url, append index.html

        if (job->requestUrl().path()==QStringLiteral("/")) {
            resourceUrl.setPath(resourceUrl.path()+"index.html");
        }

        if (job->requestUrl().path().startsWith("/r/")) {
            resourceUrl.setPath("/regex101/index.html");
        }

        // serve the requested file

        QFile requestedFile(resourceUrl.toString());

        if (requestedFile.open(QFile::ReadOnly)) {
            auto type = db.mimeTypeForFile(resourceUrl.fileName());
            auto buffer = new QBuffer(job);
            auto fileBuffer = requestedFile.readAll();

            // disable native fetch function, let the javascript library implement it's own as the native fetch cannot use our custom scheme

            if (type.inherits(htmlMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

                fileString = setInitialState(fileString, job->requestUrl());

                fileString = fileString.replace(QRegularExpression(R"((http)(s{0,1}):\/\/regex101\.com)"), "regex101:/");

                fileBuffer = fileString.toUtf8();
            }

            if (type.inherits(javascriptMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

                // when the applicaton javascript is requested, we wrap it in a bridge object which stops the application
                // from executing before our replacement functions for fetch and local storage have been installed.

                if (QRegularExpression(R"((bundle\.js)|(chunk.js))").match(job->requestUrl().path()).hasMatch()) {
                    auto bridgeContent = m_injectedJavascript;

                    fileString = bridgeContent.replace(QRegularExpression(R"(\/\/\s*!{3}NEDRYSOFT_INJECT_FILE!{3}.*)"), fileString);
                }

                // hide some of the left sidebar items that aren't appropriate for a offline application

                if (job->requestUrl().path()=="/static/bundle.js") {
                    auto iconList = QStringList() << "gamepad" << "chat" << "user";

                    for (auto icon : iconList) {
                        fileString = fileString.replace(QRegularExpression(QString(R"(icon:(\s*r\s*\?\s*void\s*0\s*:)?\s*"%1",)").arg(icon)), QString(R"(icon:"%1",style:{display:"none"},)").arg(icon));;
                    }
                }

                fileString = fileString.replace(QRegularExpression(R"(https:\/\/srv\.buysellads\.com\/)"), root());

                fileString = fileString.replace(QRegularExpression(R"(className:\s*ep\.a\.sponsorText)"), "style:{display:\"none\"},className:ep.a.sponsorText");

                fileString = fileString.replace(QRegularExpression(R"((http)(s{0,1}):\\/\\/regex101\.com)"), "regex101:/");

                fileBuffer = fileString.toUtf8();
            }

            buffer->open(QIODevice::WriteOnly);
            buffer->write(fileBuffer);
            buffer->close();

            connect(job, &QObject::destroyed, buffer, &QObject::deleteLater);

            job->reply(type.name().toUtf8(), buffer);
        }  else {
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        }
    } else {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
    }
}

QString Nedrysoft::RegExUrlSchemeHandler::setInitialState(QString fileContent, QUrl requestUrl)
{
    QJsonObject initialState, regexEditor, matchResult, unitTests, editTest, general, account, regexLibrary,libraryEntry,quiz;
    auto match = QRegularExpression(R"(\/r\/(?P<permalinkFragment>.*)\/(?P<version>\d+))").match(requestUrl.path());

    auto settingsData = Nedrysoft::RegExApiEndpoint::getInstance()->localStorageGetItem("regex101-state").toByteArray();

    auto jsonDocument = QJsonDocument::fromJson(settingsData);

    initialState = jsonDocument.object();

    matchResult["data"] = QJsonArray();
    matchResult["time"] = 0;

    if (!initialState.contains("regexEditor")) {
        initialState["regexEditor"] = QJsonObject();
    } else {
        regexEditor = initialState["regexEditor"].toObject();
    }

    if (!initialState.contains("libraryEntry")) {
        initialState["libraryEntry"] = QJsonObject();
    } else {
        libraryEntry = initialState["libraryEntry"].toObject();
    }

    if (!initialState.contains("general")) {
        initialState["general"] = QJsonObject();
    } else {
        general = initialState["general"].toObject();
    }

    if (!match.hasMatch()) {
        regexEditor["flavor"] = "pcre";
        regexEditor["delimiter"] = "/";
        regexEditor["flags"] = "gm";
        regexEditor["regex"] = "";
        regexEditor["testString"] = "";
        regexEditor["matchResult"] = matchResult;

        libraryEntry["title"] = QJsonValue::Null;
        libraryEntry["description"] = QJsonValue::Null;
        libraryEntry["author"] = QJsonValue::Null;

        general["permalinkFragment"] = QJsonValue::Null;
        general["version"] = QJsonValue::Null;
    } else {
        Nedrysoft::RegExApiEndpoint::getInstance()->regex(initialState, match.captured("permalinkFragment"), match.captured("version").toInt());

        regexEditor = initialState["regexEditor"].toObject();
        general = initialState["general"].toObject();
        libraryEntry = initialState["libraryEntry"].toObject();
    }

    general["deleteCode"] = QJsonValue::Null;
    general["userId"] = QJsonValue::Null;
    general["email"] = QJsonValue::Null;
    general["profilePicture"] = QJsonValue::Null;
    general["serviceProvider"] = QJsonValue::Null;
    general["isFavorite"] = false;
    general["isLibraryEntry"] = false;
    general["cookie"] = QJsonValue::Null;
    general["sponsorData"] = QJsonValue::Null;

    auto database = RegExDatabase::getInstance();

    auto query = database->prepareQuery("getUserDetails");

    query.bindValue(":userId", 1);

    if (query.exec()) {
        if (query.first()) {
            general["userId"] = query.value("userId").toString();
            general["email"] = query.value("email").toString();
        }
    }

    if (general.count()) {
        initialState["general"] = general;
    }

    if (regexEditor.count()) {
        initialState["regexEditor"] = regexEditor;
    }

    initialState["libraryEntry"] = libraryEntry;

    // set up unit tests data

    if (!initialState.contains("unitTests")) {
        initialState["unitTests"] = QJsonObject();
    } else {
        unitTests = initialState["unitTests"].toObject();
    }

    editTest["test"] = QJsonObject();
    editTest["id"] = -1;

    unitTests["tests"] = QJsonArray();
    unitTests["editTest"] = editTest;
    unitTests["testsRunning"] = false;

    initialState["unitTests"] = unitTests;

    // set up account information

    if (!initialState.contains("account")) {
        initialState["account"] = QJsonObject();
    } else {
        account = initialState["account"].toObject();
    }

    account["data"] = QJsonArray();
    account["pages"] = 0;
    account["allTags"] = QJsonArray();
    account["staleData"] = true;

    initialState["account"] = account;

    // set up & store library information

    if (!initialState.contains("regexLibrary")) {
        initialState["regexLibrary"] = QJsonObject();
    } else {
        regexLibrary = initialState["regexLibrary"].toObject();
    }

    regexLibrary["libraryData"] = QJsonArray();
    regexLibrary["pages"] =  0;
    regexLibrary["details"] = QJsonValue::Null;

    initialState["regexLibrary"] = regexLibrary;

    // set up & store quiz information

    if (!initialState.contains("quiz")) {
        initialState["quiz"] = QJsonObject();
    } else {
        quiz = initialState["quiz"].toObject();
    }

    quiz["allTasks"] = QJsonArray();
    quiz["mostRecentTaskIdx"] = QJsonValue::Null;

    initialState["quiz"] = quiz;

    auto initialStateString = QUrl::toPercentEncoding(QJsonDocument(initialState).toJson(QJsonDocument::Compact));

    return fileContent.replace(QRegularExpression(R"(nedrySoft\.initialState)"), QString::fromUtf8(initialStateString));
}
