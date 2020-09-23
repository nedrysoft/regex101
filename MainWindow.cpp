#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtWebEngineWidgets>
#include <QMimeDatabase>
#include <QMimeType>
#include <QRegularExpression>
#include <QSplashScreen>

QString RegExUrlSchemeHandler::name()
{
    return(QStringLiteral("regex101"));
}

RegExUrlSchemeHandler::RegExUrlSchemeHandler(QString resourceRoolFolder)
{
    m_resourceRootFolder = resourceRoolFolder;
}

void RegExUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    QMimeDatabase db;

    static const QByteArray GET(QByteArrayLiteral("GET"));

    QByteArray method = job->requestMethod();
    QUrl url = job->requestUrl();

    if (method == GET) {
        QUrl resourceUrl = job->requestUrl();

        resourceUrl.setScheme(":/");
        resourceUrl.setPath(m_resourceRootFolder+resourceUrl.path());

        if (job->requestUrl().path()=="/") {
            resourceUrl.setPath(resourceUrl.path()+"index.html");
        }

        QFile f(resourceUrl.toString());

        if (f.open(QFile::ReadOnly)) {
            QMimeType type = db.mimeTypeForFile(resourceUrl.fileName());
            QBuffer *buffer = new QBuffer(job);
            QByteArray fileBuffer = f.readAll();

            if (type.inherits("text/css")) {
                auto fileString = QString::fromUtf8(fileBuffer);

                // hide the left hand panel

                fileString = fileString.replace(QRegularExpression("(?i)\\._2eQHI\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}"),"._2eQHI{display:none;}");

                // hide the top bar items

                fileString = fileString.replace(QRegularExpression("(?i)\\_2PLNZ\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}"),"_2PLNZ{display:none;}");

                // align selection panel to the left

                fileString = fileString.replace(QRegularExpression("(?i)(\\._3tDL-)\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}"),"._3tDL-{margin-left=0}");

                // sponsors div

                fileString = fileString.replace(QRegularExpression("(?i)\\.tNf50\\{([a-z]|[0-9]|\\;|\\:|\\#|\\s|\\-|\\%|\\*)*\\}"),".tNf50{display:none;}");

                fileBuffer = fileString.toUtf8();
            }

            if ((type.inherits("text/html")) || (type.inherits("text/javascript"))) {
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

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    if (info.requestUrl().scheme()!=RegExUrlSchemeHandler::name()) {
        info.block(true);
    }
}

MainWindow::MainWindow(QSplashScreen *splashScreen, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    RequestInterceptor *interceptor = new RequestInterceptor;

    ui->widget->page()->profile()->setUrlRequestInterceptor(interceptor);

    ui->widget->setUrl(QUrl(RegExUrlSchemeHandler::name()+":/"));

    this->showMaximized();

    connect(ui->widget->page(), &QWebEnginePage::loadFinished, [=](bool finished) {
        if (finished) {
            QTimer::singleShot(1000, [=]() {
                splashScreen->finish(this);
            });
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

