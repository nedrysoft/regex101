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

#include "SettingsDialog.h"
#include "TransparentWidget.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#if defined(Q_OS_MACOS)
#include <QtMac>
#include <QMacToolBar>
#endif
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#if defined(Q_OS_MACOS)
#include "MacHelper.h"

constexpr auto transisionDuration = 100;
#else
constexpr auto categoryFontSize = 24;
constexpr auto settingsTreeWidth = 144;
constexpr auto settingsIconSize = 32;
#endif

Nedrysoft::SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent)
{
#if defined(Q_OS_MACOS)
    m_toolBar = new QMacToolBar(this);

    resize(600,400);

    auto databaseIcon = QIcon("://assets/Gianni-Polito-Colobrush-System-database.icns");
    auto generalIcon = QIcon(Nedrysoft::MacHelper::macStandardImage(Nedrysoft::StandardImage::NSImageNamePreferencesGeneral, QSize(256,256)));
#else
    m_layout = new QHBoxLayout;

    m_treeWidget = new QTreeWidget(this);

    m_treeWidget->setIndentation(0);

    m_treeWidget->setMinimumWidth(settingsTreeWidth);
    m_treeWidget->setMaximumWidth(settingsTreeWidth);
    m_treeWidget->setIconSize(QSize(settingsIconSize, settingsIconSize));

    m_treeWidget->setHeaderHidden(true);

    m_treeWidget->setSelectionBehavior(QTreeWidget::SelectRows);

    m_treeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    m_stackedWidget = new QStackedWidget;

    m_stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout->addWidget(m_treeWidget);

    m_categoryLabel = new QLabel;

    m_categoryLabel->setFont(QFont(m_categoryLabel->font().family(), categoryFontSize));
    m_categoryLabel->setAlignment(Qt::AlignLeft);

    m_detailLayout = new QVBoxLayout;

    m_detailLayout->addWidget(m_categoryLabel);
    m_detailLayout->addWidget(m_stackedWidget);

    m_detailLayout->setAlignment(m_categoryLabel, Qt::AlignLeft);

    m_layout->addLayout(m_detailLayout);

    setLayout(m_layout);

    auto databaseIcon = QIcon(":/assets/noun_database_2757856.png");
    auto generalIcon = QIcon(":/assets/noun_Settings_716654.png");
#endif
    addPage(tr("General"), tr("General settings"), generalIcon, new QLabel("HELLO"));
    addPage(tr("Database"), tr("The database settings"), databaseIcon, new QLabel("GOODBYE"));

#if defined(Q_OS_MACOS)
    m_toolBar->attachToWindow(nativeWindowHandle());
#endif
}

void Nedrysoft::SettingsDialog::resizeEvent(QResizeEvent *event)
{
    for(auto page : m_pages) {
        if (page->m_widget) {
            page->m_widget->resize(event->size());
        }
    }
}

QWindow *Nedrysoft::SettingsDialog::nativeWindowHandle()
{
    // @note the call to winId() is required as it sets up windowHandle() to return the correct value,
    //       failing to call this will result in windowHandle not returning the correct value.

    this->window()->winId();

    return this->window()->windowHandle();
}

void Nedrysoft::SettingsDialog::addPage(QString name, QString description, QIcon icon, QWidget *widget)
{
#if defined(Q_OS_MACOS)
    //auto widgetContainer = new TransparentWidget(widget, 1, this);

    //m_widgets.append(primary);
    //m_widgets.append(secondary);

    //auto transparencyEffect = widgetContainer->transparencyEffect();

    QMacToolBarItem *toolBarItem = m_toolBar->addItem(icon, name);

    connect(toolBarItem, &QMacToolBarItem::activated, this, [this]() {
        Q_UNUSED(this);
        /*
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");

        animation->setDuration(transisionDuration);
        animation->setStartValue(this->frameGeometry());
        animation->setEndValue(this->frameGeometry().adjusted(-100, -100, 100, 100));

        QParallelAnimationGroup *group = new QParallelAnimationGroup;

        group->addAnimation(animation);

        QPropertyAnimation *fo = new QPropertyAnimation(primaryEffect,"opacity");

        fo->setDuration(transisionDuration);
        fo->setStartValue(1);
        fo->setEndValue(0);
        group->addAnimation(fo);

        QPropertyAnimation *fi = new QPropertyAnimation(secondaryEffect,"opacity");

        fi->setDuration(transisionDuration);
        fi->setStartValue(0);
        fi->setEndValue(1);

        group->addAnimation(fi);

        group->start();*/
    });
#else
    QTreeWidgetItem *newTreeItem = new QTreeWidgetItem;

    newTreeItem->setIcon(0, icon);
    newTreeItem->setText(0, name);
    newTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(widget));
    newTreeItem->setData(0, Qt::ToolTipRole, description);

    m_treeWidget->addTopLevelItem(newTreeItem);

    m_stackedWidget->addWidget(widget);

    connect(m_treeWidget, &QTreeWidget::currentItemChanged, [=](QTreeWidgetItem *current, [[maybe_unused]] QTreeWidgetItem *previous) {
        auto widget = current->data(0, Qt::UserRole).value<QWidget *>();

        if (widget) {
            m_stackedWidget->setCurrentWidget(widget);
            m_categoryLabel->setText(current->text(0));
        }
    });
#endif
}
