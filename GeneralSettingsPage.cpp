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

#include "GeneralSettingsPage.h"
#include "MainWindow.h"
#include "ui_GeneralSettingsPage.h"

#include <QDebug>
#include <QMessageBox>

Nedrysoft::GeneralSettingsPage::GeneralSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsPage)
{
    ui->setupUi(this);

#if defined(Q_OS_MACOS)
    m_size = QSize(qMax(minimumSizeHint().width(), size().width()), qMax(minimumSizeHint().height(), size().height()));
#else
    m_size = minimumSizeHint();
#endif

    connect(ui->updateClonePushButton, &QPushButton::clicked, [=](bool /*checked*/) {
        if (QMessageBox::warning(MainWindow::getInstance(), "Update Clone", tr("Updating the clone may break functionality.\r\n\r\nYou can revert to the factory supplied clone which has been tested for compatability by clicking the \"Reset to Factory Clone\" button if you experience problems.\r\n\r\nDo you want to update the clone?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No)==QMessageBox::Yes) {
            // TODO: Do factory clone
        }
    });

    connect(ui->resetToFactoryClonePushButton, &QPushButton::clicked, [=](bool /*checked*/) {
        if (QMessageBox::warning(MainWindow::getInstance(), "Update Clone", tr("Are you sure you want to revent back to the factory clone?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No)==QMessageBox::Yes) {
            // TODO: Delete user clone
        }
    });

    ui->resetToFactoryClonePushButton->setEnabled(false);
}

Nedrysoft::GeneralSettingsPage::~GeneralSettingsPage()
{
    delete ui;
}

bool Nedrysoft::GeneralSettingsPage::canAcceptSettings()
{
    return true;
}

void Nedrysoft::GeneralSettingsPage::acceptSettings()
{
}

QSize Nedrysoft::GeneralSettingsPage::sizeHint() const
{
    return m_size;
}
