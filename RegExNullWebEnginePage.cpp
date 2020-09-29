#include "RegExNullWebEnginePage.h"
#include <QDesktopServices>

Nedrysoft::RegExNullWebEnginePage::RegExNullWebEnginePage()
{
}

bool Nedrysoft::RegExNullWebEnginePage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    Q_UNUSED(isMainFrame);

    if (type==QWebEnginePage::NavigationTypeLinkClicked) {
        QDesktopServices::openUrl(url);
    }

    this->deleteLater();

    return false;
}
