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
#include "ui_MainWindow.h"
#include "RegExAboutDialog.h"
#include "RegExWebEnginePage.h"
#include "RegExSplashScreen.h"
#include <QtWebEngineWidgets>

MainWindow::MainWindow(RegExSplashScreen *splashScreen, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_page(new RegExWebEnginePage)
{
    ui->setupUi(this);

    this->showMaximized();

    ui->widget->setPage(m_page);

    connect(ui->widget->page(), &QWebEnginePage::loadFinished, splashScreen, [=](bool finished) {
        if (finished) {
            QTimer::singleShot(1000, splashScreen, [=]() {
                splashScreen->close();
            });
        }
    });
}

MainWindow::~MainWindow()
{
    delete m_page;
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    RegExAboutDialog aboutDialog(this);

    aboutDialog.exec();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}
