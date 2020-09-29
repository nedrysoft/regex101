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
constexpr auto disableFetchJavascript = R"(<script type="text/javascript" src="./qwebchannel.js"></script><script src="/regexbridge.js"></script>)";

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

    if (method == GET) {
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
