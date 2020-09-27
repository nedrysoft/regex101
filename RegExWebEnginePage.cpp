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

#include <QWebChannel>
#include "RegExWebEnginePage.h"
#include "RegExWebEngineProfile.h"
#include <QTimer>

RegExWebEnginePage::RegExWebEnginePage() :
    QWebEnginePage(new RegExWebEngineProfile),
    m_urlInterceptor(new RegExUrlRequestInterceptor)
{
    m_profile = dynamic_cast<RegExWebEngineProfile *>(profile());

    m_apiEndpoint = new RegExApiEndpoint;
    m_apiChannel = new QWebChannel;

    setWebChannel(m_apiChannel);

    m_apiChannel->registerObject(QString("RegExApiEndpoint"), m_apiEndpoint);

    setUrlRequestInterceptor(m_urlInterceptor);

    setUrl(QUrl(RegExUrlSchemeHandler::name()+":/"));
}

RegExWebEnginePage::~RegExWebEnginePage()
{
    m_profile->deleteLater();
}

void RegExWebEnginePage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    Q_UNUSED(level);
    Q_UNUSED(lineNumber);
    Q_UNUSED(sourceID);
    Q_UNUSED(message);

    //qDebug() << message;
}
