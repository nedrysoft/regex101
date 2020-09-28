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
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QUrlQuery>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>

namespace Nedrysoft {
    struct RegExPair {
        const char *expression;
        const char *replacement;
    } RegExPair;
}

struct Nedrysoft::RegExPair sylesheetRegularExpressions[] = {
    {"(?i)\\._2eQHI\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "._2eQHI{display:none;}"},          // left hand panel
    {"(?i)\\_2PLNZ\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "_2PLNZ{display:none;}"},            // top bar items
    {"(?i)(\\._3tDL-)\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "._3tDL-{margin-left=0}"},        // fix left panel position
    {"(?i)\\.tNf50\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", ".tNf50{display:none;}"},            // sponsors div
};

constexpr auto advertResponse = "({\"ads\":[{}]});";
constexpr auto htmlMimeType = "text/html";
constexpr auto cssMimeType = "text/css";
constexpr auto javascriptMimeType = "application/javascript";
constexpr auto qrcRootFolder = ":/";
constexpr auto disableFetchJavascript = "<script type=\"text/javascript\" src=\"./qwebchannel.js\"></script><script src=\"/regexbridge.js\"></script>";

auto GET(QByteArrayLiteral("GET"));
auto POST(QByteArrayLiteral("POST"));
auto DELETE(QByteArrayLiteral("DELETE"));

QString Nedrysoft::RegExUrlSchemeHandler::name()
{
    return QStringLiteral("regex101");
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

        if (job->requestUrl().path()=="/ads/CKYDE5QY.json") {
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

        if (job->requestUrl().path()=="/") {
            resourceUrl.setPath(resourceUrl.path()+"index.html");
        }

        // serve the requested file

        QFile f(resourceUrl.toString());

        if (f.open(QFile::ReadOnly)) {
            auto type = db.mimeTypeForFile(resourceUrl.fileName());
            auto buffer = new QBuffer(job);
            auto fileBuffer = f.readAll();

            // hide/move various sections of the original page by modifying the css

            if (type.inherits(cssMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

                for (auto regExPair : sylesheetRegularExpressions) {
                    fileString = fileString.replace(QRegularExpression(regExPair.expression), regExPair.replacement);
                }

                fileBuffer = fileString.toUtf8();
            }

            // disable native fetch function, let the javascript library implement it's own as the native fetch cannot use our custom scheme

            if (type.inherits(htmlMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

                fileString = disableFetchJavascript+fileString;

                fileBuffer = fileString.toUtf8();
            }

            if (type.inherits(javascriptMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);

                fileString = fileString.replace(QRegularExpression("https://srv.buysellads.com/"), "regex101:/");

                fileBuffer = fileString.toUtf8();
            }

            buffer->open(QIODevice::WriteOnly);
            buffer->write(fileBuffer);
            buffer->close();

            connect(job, &QObject::destroyed, buffer, &QObject::deleteLater);

            job->reply(type.name().toLatin1(), buffer);
        }  else {
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        }
    } else {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
    }
}
