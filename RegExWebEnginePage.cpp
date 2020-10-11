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

#include "RegExNullWebEnginePage.h"
#include "RegExUrlSchemeHandler.h"
#include "RegExWebEnginePage.h"
#include "RegExWebEngineProfile.h"

#include <QTimer>
#include <QWebChannel>
#include <QWindow>
#include <QGridLayout>

#include <QFile>

Nedrysoft::RegExWebEnginePage::RegExWebEnginePage() :
    QWebEnginePage(new Nedrysoft::RegExWebEngineProfile),
    m_urlInterceptor(new Nedrysoft::RegExUrlRequestInterceptor)
{
    QUrl url(RegExUrlSchemeHandler::root());

    m_profile = dynamic_cast<RegExWebEngineProfile *>(profile());

    m_apiChannel = new QWebChannel;

    setWebChannel(m_apiChannel);

    m_apiChannel->registerObject(QString("RegExApiEndpoint"), RegExApiEndpoint::getInstance());

    setUrlRequestInterceptor(m_urlInterceptor);

    setUrl(url);
}

Nedrysoft::RegExWebEnginePage::~RegExWebEnginePage()
{
    m_profile->deleteLater();
    m_urlInterceptor->deleteLater();
}

void Nedrysoft::RegExWebEnginePage::javaScriptConsoleMessage([[maybe_unused]] JavaScriptConsoleMessageLevel level, [[maybe_unused]] const QString &message, [[maybe_unused]] int lineNumber, [[maybe_unused]] const QString &sourceID)
{
    qDebug() << message;
}

void Nedrysoft::RegExWebEnginePage::javaScriptAlert([[maybe_unused]] const QUrl &securityOrigin, [[maybe_unused]] const QString &msg)
{
}

QWebEnginePage *Nedrysoft::RegExWebEnginePage::createWindow([[maybe_unused]] QWebEnginePage::WebWindowType type)
{
    return new Nedrysoft::RegExNullWebEnginePage();
}

bool Nedrysoft::RegExWebEnginePage::acceptNavigationRequest(const QUrl &url, [[maybe_unused]] QWebEnginePage::NavigationType type, [[maybe_unused]] bool isMainFrame)
{
    if (url.scheme()!=Nedrysoft::RegExUrlSchemeHandler::name()) {
        return false;
    }

    return true;
}

