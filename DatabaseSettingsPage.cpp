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

#include "DatabaseSettingsPage.h"
#include "RegExDatabase.h"
#include "ui_DatabaseSettingsPage.h"

#include <QDebug>
#include <QMap>
#include <QSqlDatabase>

Nedrysoft::DatabaseSettingsPage::DatabaseSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseSettingsPage)
{
    ui->setupUi(this);

    QMap<QString, QString> databaseDriverMap(std::map<QString, QString> {
        {"QDB2", "IBM DB2 (Version 7.1 and above)"},
        {"QIBASE", "Borland InterBase" },
        {"QMYSQL", "MySQL or MariaDB (Version 5.0 and above)"},
        {"QOCI", "Oracle Call Interface Driver"},
        {"QODBC", "ODBC"},
        {"QODBC3", "ODBC (Version 3 and above)"},
        {"QPSQL", "PostgreSQL (Version 7.3 and above)"},
        {"QPSQL7", "PostgreSQL (Version 6.x and 7.x)"},
        {"QSQLITE2", "SQLite (Version 2)"},
        {"QSQLITE", "SQLite (Version 3)"},
        {"QTDS", "Sybase Adaptive Server"}
    });

    auto ignoredDriverList = QStringList() << "QODBC" << "QPSQL7" << "QSQLITE2";

#if defined(Q_OS_MACOS)
    m_size = QSize(qMax(minimumSizeHint().width(), size().width()), qMax(minimumSizeHint().height(), size().height()));
#else
    m_size = minimumSizeHint();
#endif

    auto driverList = QSqlDatabase::drivers();

    for(auto driver : driverList) {
        if ((ignoredDriverList.contains(driver))) {
            continue;
        }

        if (databaseDriverMap.contains(driver)) {
            ui->databaseTypeComboBox->addItem(databaseDriverMap[driver]+"  ", driver);;
        } else {
            ui->databaseTypeComboBox->addItem(driver, driver);
        }
    }

    QVariantMap settingsMap;

    settingsMap = RegExDatabase::getInstance()->getSettings();

    ui->databaseTypeComboBox->setCurrentText(settingsMap["databaseDriver"].toString());
    ui->databaseNameLineEdit->setText(settingsMap["databaseName"].toString());
    ui->hostNameLineEdit->setText(settingsMap["databaseHost"].toString());
    ui->usernameLineEdit->setText(settingsMap["databaseUser"].toString());
    ui->passwordLineEdit->setText(settingsMap["databasePassword"].toString());
}

Nedrysoft::DatabaseSettingsPage::~DatabaseSettingsPage()
{
    delete ui;
}

bool Nedrysoft::DatabaseSettingsPage::canAcceptSettings()
{
    return true;
}

void Nedrysoft::DatabaseSettingsPage::acceptSettings()
{    
    QVariantMap settingsMap;

    settingsMap["databaseDriver"] = ui->databaseTypeComboBox->currentData();
    settingsMap["databaseName"] = ui->databaseNameLineEdit->text();
    settingsMap["databaseHost"] = ui->hostNameLineEdit->text();
    settingsMap["databaseUser"] = ui->usernameLineEdit->text();
    settingsMap["databasePassword"]= ui->passwordLineEdit->text();

    RegExDatabase::getInstance()->storeSettings(settingsMap);
}

QSize Nedrysoft::DatabaseSettingsPage::sizeHint() const
{
    return m_size;
}
