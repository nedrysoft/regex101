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

#include "RegExUrlSchemeHandler.h"

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
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

        if (QRegularExpression(R"(\/ads\/.*.json)").match(job->requestUrl().path()).hasMatch()) {
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

                fileString = fileString.replace(QRegularExpression("(http)(s{0,1}):\\/\\/regex101.com"), "regex101:/");

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

                fileString = fileString.replace(QRegularExpression("https://srv.buysellads.com/"), root());

                fileString = fileString.replace(QRegularExpression(R"(className:\s*ep.a.sponsorText)"), "style:{display:\"none\"},className:ep.a.sponsorText");

                fileString = fileString.replace(QRegularExpression("(http)(s{0,1}):\\/\\/regex101.com"), "regex101:/");

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

    QString initialState = "%7B%22rightSidebar%22%3A%7B%22activeSubsection%22%3A%22EXPLANATION%22%2C%22visibleSections%22%3A%5B%22EXPLANATION%22%2C%22MATCH_INFO%22%2C%22QUICKREF%22%5D%2C%22hiddenSections%22%3A%5B%5D%2C%22width%22%3Anull%7D%2C%22regexEditor%22%3A%7B%22flavor%22%3A%22javascript%22%2C%22delimiter%22%3A%22%2F%22%2C%22flags%22%3A%22gm%22%2C%22regex%22%3A%22((%5C%5Cu00a9%7C%5C%5Cu00ae%7C%5B%5C%5Cu2000-%5C%5Cu3300%5D%7C%5C%5Cud83c%5B%5C%5Cud000-%5C%5Cudfff%5D%7C%5C%5Cud83d%5B%5C%5Cud000-%5C%5Cudfff%5D%7C%5C%5Cud83e%5B%5C%5Cud000-%5C%5Cudfff%5D%5C%5Cs%3F)%2B)%22%2C%22testString%22%3A%22%F0%9F%A7%9C%E2%80%8D%E2%99%80%EF%B8%8F%22%2C%22matchResult%22%3A%7B%22data%22%3A%5B%5D%2C%22time%22%3A0%7D%2C%22error%22%3Anull%2C%22substString%22%3A%22%241%22%2C%22hasUnsavedData%22%3Afalse%2C%22regexVersions%22%3A0%2C%22showMatchArea%22%3Afalse%2C%22showSubstitutionArea%22%3Atrue%2C%22showUnitTestArea%22%3Afalse%7D%2C%22settings%22%3A%7B%22maxExecutionTime%22%3A2000%2C%22theme%22%3A%22light%22%2C%22nonParticipatingGroups%22%3Afalse%2C%22displayWhitespace%22%3Afalse%2C%22colorizeSyntax%22%3Atrue%2C%22autoComplete%22%3Afalse%2C%22wrapLines%22%3Atrue%2C%22testAreaLineNumbers%22%3Afalse%2C%22regexAreaLineNumbers%22%3Afalse%2C%22language%22%3A%22ENGLISH%22%2C%22storageSaved%22%3Anull%2C%22storageLoaded%22%3Anull%2C%22editorTooltips%22%3Atrue%2C%22alwaysCollapseLeftSidebar%22%3Afalse%2C%22alwaysCollapseRightSidebar%22%3Afalse%2C%22defaultFlavor%22%3A%22pcre%22%2C%22showWarningUnsavedProgress%22%3Atrue%7D%2C%22unitTests%22%3A%7B%22tests%22%3A%5B%5D%2C%22editTest%22%3A%7B%22test%22%3A%7B%7D%2C%22id%22%3A-1%7D%2C%22testsRunning%22%3Afalse%7D%2C%22general%22%3A%7B%22permalinkFragment%22%3A%22ebZRGK%22%2C%22version%22%3A1%2C%22deleteCode%22%3Anull%2C%22userId%22%3Anull%2C%22email%22%3Anull%2C%22profilePicture%22%3Anull%2C%22serviceProvider%22%3Anull%2C%22isFavorite%22%3Afalse%2C%22isLibraryEntry%22%3Atrue%2C%22title%22%3Anull%2C%22cookie%22%3A%22_ga%3DGA1.2.1171425424.1599835688%3B%20_gid%3DGA1.2.1060624053.1601204614%22%2C%22sponsorData%22%3Anull%2C%22error%22%3Anull%7D%2C%22account%22%3A%7B%22data%22%3A%5B%5D%2C%22pages%22%3A0%2C%22allTags%22%3A%5B%5D%2C%22staleData%22%3Atrue%7D%2C%22regexLibrary%22%3A%7B%22libraryData%22%3A%5B%5D%2C%22pages%22%3A0%2C%22details%22%3Anull%7D%2C%22libraryEntry%22%3A%7B%22title%22%3A%22Match%20emojis%20in%20source%22%2C%22description%22%3A%22Use%20to%20match%20source%20text%20emojis.%22%2C%22author%22%3A%22%22%7D%2C%22quiz%22%3A%7B%22allTasks%22%3A%5B%5D%2C%22mostRecentTaskIdx%22%3Anull%7D%7D";

    return(fileContent.replace(QRegularExpression(R"(nedrySoft\.initialState)"), initialState));
}
