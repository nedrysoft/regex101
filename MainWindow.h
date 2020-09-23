#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineUrlRequestInterceptor>
#include <QDebug>
#include <QThread>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSplashScreen;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSplashScreen *splashScreen, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSplashScreen *m_splashScreen;
};

class RegExUrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    RegExUrlSchemeHandler(QString resourceRoolFolder);
    void requestStarted(QWebEngineUrlRequestJob *request);

    static QString name();

private:
    QString m_resourceRootFolder;
};

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    void interceptRequest(QWebEngineUrlRequestInfo &info);
};


#endif // MAINWINDOW_H
