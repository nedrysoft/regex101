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

#include "MainWindow.h"
#include "RegExAboutDialog.h"
#include "RegExWebEnginePage.h"
#include "RegExSplashScreen.h"
#include "SettingsDialog.h"
#include "ui_MainWindow.h"

#include <chrono>
#include <QDebug>
#include <QtWebEngineWidgets>

using namespace std::chrono_literals;

constexpr auto splashScreenDuration = 1s;

Nedrysoft::MainWindow *Nedrysoft::MainWindow::m_instance = nullptr;

Nedrysoft::MainWindow::MainWindow(Nedrysoft::RegExSplashScreen *splashScreen)
    : QMainWindow(nullptr),
      ui(new Ui::MainWindow),
      m_page(new Nedrysoft::RegExWebEnginePage),
      m_settingsDialog(nullptr)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);

    m_instance = this;

    showMaximized();

    ui->webEngineView->setPage(m_page);

    connect(ui->webEngineView->page(), &QWebEnginePage::loadFinished, splashScreen, [=](bool finished) {
        if (finished) {
            QTimer::singleShot(splashScreenDuration, splashScreen, [=]() {
                splashScreen->close();
            });
        }
    });

    QDesktopServices::setUrlHandler(RegExUrlSchemeHandler::name(), this, SLOT("handleOpenByUrl"));
}

Nedrysoft::MainWindow::~MainWindow()
{
    delete m_page;
    delete ui;
}

Nedrysoft::MainWindow *Nedrysoft::MainWindow::getInstance()
{
    return(m_instance);
}

void Nedrysoft::MainWindow::on_actionAbout_triggered()
{
    Nedrysoft::RegExAboutDialog aboutDialog(this);

    aboutDialog.exec();
}

void Nedrysoft::MainWindow::on_actionExit_triggered()
{
    close();
}

void Nedrysoft::MainWindow::handleOpenByUrl([[maybe_unused]] const QUrl &url)
{
}

bool Nedrysoft::MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type()==QEvent::FileOpen) {
        QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent*>(event);

        if (!fileOpenEvent->url().isEmpty()) {
            // fileOpenEvent->url() contains the url if launched via url scheme
        } else if (!fileOpenEvent->file().isEmpty()) {
            // fileOpenEvent->file() contains the filename if launched via file association
        }

        return false;
    }

    return QObject::eventFilter(obj, event);
}

void Nedrysoft::MainWindow::on_actionPreferences_triggered()
{
    if (m_settingsDialog) {
        m_settingsDialog->raise();

        return;
    }

    m_settingsDialog = new SettingsDialog(this);

    m_settingsDialog->show();

    connect(m_settingsDialog, &SettingsDialog::closed, [=](){
        m_settingsDialog->deleteLater();

        m_settingsDialog = nullptr;
    });
}

void Nedrysoft::MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    if (m_settingsDialog) {
        m_settingsDialog->close();
        m_settingsDialog->deleteLater();
        m_settingsDialog = nullptr;
    }

    closeEvent->accept();
}
