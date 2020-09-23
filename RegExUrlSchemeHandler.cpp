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
#include <QFile>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>

constexpr const char *sylesheetRegularExpressions[] = {
    "(?i)\\._2eQHI\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "._2eQHI{display:none;}",          // left hand panel
    "(?i)\\_2PLNZ\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "_2PLNZ{display:none;}",            // top bar items
    "(?i)(\\._3tDL-)\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", "._3tDL-{margin-left=0}",        // fix left panel position
    "(?i)\\.tNf50\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}", ".tNf50{display:none;}",            // sponsors div
    nullptr
};

constexpr const char *htmlMimeType = "text/html";
constexpr const char *javascriptMimeType = "text/javascript";
constexpr const char *cssMimeType = "text/css";
constexpr const char *qrcRootFolder = ":/";

QString RegExUrlSchemeHandler::name()
{
    return QStringLiteral("regex101");
}

void RegExUrlSchemeHandler::registerScheme()
{
    QWebEngineUrlScheme scheme((RegExUrlSchemeHandler::name().toUtf8()));

    scheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme | QWebEngineUrlScheme::LocalAccessAllowed | QWebEngineUrlScheme::ContentSecurityPolicyIgnored);

    QWebEngineUrlScheme::registerScheme(scheme);
}

RegExUrlSchemeHandler::RegExUrlSchemeHandler(QString resourceRootFolder)
{
    m_resourceRootFolder = resourceRootFolder;
}

void RegExUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    QMimeDatabase db;

    static const QByteArray GET(QByteArrayLiteral("GET"));

    QByteArray method = job->requestMethod();

    if (method == GET) {
        QUrl resourceUrl = job->requestUrl();

        resourceUrl.setScheme(qrcRootFolder);
        resourceUrl.setPath(m_resourceRootFolder+resourceUrl.path());

        // if there was no file in the url, append index.html

        if (job->requestUrl().path()=="/") {
            resourceUrl.setPath(resourceUrl.path()+"index.html");
        }

        // serve the requested file

        QFile f(resourceUrl.toString());

        if (f.open(QFile::ReadOnly)) {
            QMimeType type = db.mimeTypeForFile(resourceUrl.fileName());
            QBuffer *buffer = new QBuffer(job);
            QByteArray fileBuffer = f.readAll();

            // check if we are serving the css file, if so we need to modify it to hide/move various sections of the original page

            if (type.inherits(cssMimeType)) {
                auto fileString = QString::fromUtf8(fileBuffer);
                int currentRegexIndex = 0;

                while(sylesheetRegularExpressions[currentRegexIndex]) {
                    fileString = fileString.replace(QRegularExpression(sylesheetRegularExpressions[currentRegexIndex]), sylesheetRegularExpressions[currentRegexIndex+1]);

                    currentRegexIndex += 2;
                }

                fileBuffer = fileString.toUtf8();
            }

            // do any processing required on html or javascript files

            if ((type.inherits(htmlMimeType)) || (type.inherits(javascriptMimeType))) {
                auto fileString = QString::fromUtf8(fileBuffer);

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
    }
}
