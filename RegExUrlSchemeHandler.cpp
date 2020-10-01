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
#include "RegExUrlSchemeHandler.h"

#include <QBuffer>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QUrlQuery>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>

constexpr auto advertResponse = R"(({"ads":[{}]});)";
constexpr auto htmlMimeType = "text/html";
constexpr auto javascriptMimeType = "application/javascript";
constexpr auto qrcRootFolder = ":/";
constexpr auto disableFetchJavascript = R"(<script type="text/javascript" src="/qwebchannel.js"></script><script src="/regexbridge.js"></script>)";

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

                fileString = disableFetchJavascript+fileString;
                fileString = setInitialState(fileString, job->requestUrl());

                fileString = fileString.replace(QRegularExpression(R"((http)(s{0,1}):\/\/regex101\.com)"), "regex101:/");

                fileBuffer = fileString.toUtf8();
            }

            if (type.inherits(javascriptMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

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
    Q_UNUSED(requestUrl);
    QVariantMap initialState, regexEditor, matchResult, unitTests, editTest, general, account, regexLibrary,libraryEntry,quiz;
    auto match = QRegularExpression(R"(\/r\/(?P<permalinkFragment>.*)\/(?P<version>\d+))").match(requestUrl.path());

    auto settingsData = Nedrysoft::RegExApiEndpoint::getInstance()->localStorageGetItem("regex101-state").toByteArray();

    initialState = QJsonDocument::fromJson(settingsData).toVariant().toMap();

    matchResult["data"] = QStringList();
    matchResult["time"] = 0;

    if (!match.hasMatch()) {
        regexEditor["flavor"] = "pcre";
        regexEditor["delimiter"] = "/";
        regexEditor["flags"] = "gm";
        regexEditor["regex"] = "";
        regexEditor["testString"] = "";
        regexEditor["matchResult"] = matchResult;
    } else {
        regexEditor["flavor"] = "javascript";
        regexEditor["delimiter"] = "/";
        regexEditor["flags"] = "gm";
        regexEditor["regex"] = "((\\u00a9|\\u00ae|[\\u2000-\\u3300]|\\ud83c[\\ud000-\\udfff]|\\ud83d[\\ud000-\\udfff]|\\ud83e[\\ud000-\\udfff]\\s?)+)";
        regexEditor["testString"] = "ð§ââï";
        regexEditor["matchResult"] = matchResult;
        regexEditor["error"] = QVariant();
        regexEditor["substString"] = "$1";
        regexEditor["hasUnsavedData"] = false;
        regexEditor["regexVersions"] = 0;
        regexEditor["showMatchArea"] = false;
        regexEditor["showSubstitutionArea"] = true;
        regexEditor["showUnitTestArea"] = false;
    }

    editTest["test"] = QVariantMap();
    editTest["id"] = -1;

    unitTests["tests"] = QStringList();
    unitTests["editTest"] = editTest;
    unitTests["testsRunning"] = false;

    general["permalinkFragment"] = "ebZRGK";
    general["version"] = 1;
    general["deleteCode"] = QVariant();
    general["userId"] = QVariant();
    general["email"] = QVariant();
    general["profilePicture"] = QVariant();
    general["serviceProvider"] = QVariant();
    general["isFavorite"] = false;
    general["isLibraryEntry"] = true;
    general["title"] = QVariant();
    general["cookie"] = "_ga=GA1.2.1171425424.1599835688; _gid=GA1.2.1060624053.1601204614";
    general["sponsorData"] = QVariant();
    general["error"] = QVariant();

    account["data"] = QStringList();
    account["pages"] = 0;
    account["allTags"] = QStringList();
    account["staleData"] = true;

    regexLibrary["libraryData"] = QStringList();
    regexLibrary["pages"] =  0;
    regexLibrary["details"] =  QVariant();

    libraryEntry["title"] = "Match emojis in source";
    libraryEntry["description"] = "Use to match source text emojis.";
    libraryEntry["author"] = "";

    quiz["allTasks"] = QStringList();
    quiz["mostRecentTaskIdx"] = QVariant();

    initialState["regexEditor"] = regexEditor;
    initialState["unitTests"] = unitTests;
    initialState["general"] = general;
    initialState["account"] = account;
    initialState["regexLibrary"] = regexLibrary;
    initialState["libraryEntry"] = libraryEntry;
    initialState["quiz"] = quiz;

    auto initialStateString = QUrl::toPercentEncoding(QJsonDocument(QJsonObject::fromVariantMap(initialState)).toJson(QJsonDocument::Compact));

    return(fileContent.replace(QRegularExpression(R"(nedrySoft\.initialState)"), QString::fromUtf8(initialStateString)));
}
