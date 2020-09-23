#include "MainWindow.h"

#include <QApplication>
#include <QWebEngineUrlScheme>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QWebEngineUrlScheme scheme((RegExUrlSchemeHandler::name().toUtf8()));

    scheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme | QWebEngineUrlScheme::LocalAccessAllowed | QWebEngineUrlScheme::ContentSecurityPolicyIgnored);

    QWebEngineUrlScheme::registerScheme(scheme);

    QApplication a(argc, argv);

    QPixmap pixmap(":/assets/splash_620x300@2x.png");
    QSplashScreen splash(pixmap);

    splash.show();

    a.setApplicationDisplayName("Regular Expressions 101");
    a.setApplicationName("Regular Expressions 101");

    RegExUrlSchemeHandler handler("/regex101");

    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(RegExUrlSchemeHandler::name().toUtf8(), &handler);

    QWebEngineProfile::defaultProfile()->setHttpCacheType(QWebEngineProfile::NoCache);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    QWebEngineProfile::defaultProfile()->settings()->setUnknownUrlSchemePolicy(QWebEngineSettings::AllowAllUnknownUrlSchemes);

    MainWindow mainWindow(&splash);

    mainWindow.show();

    return a.exec();
}
