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
#include "GeneralSettingsPage.h"
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
#include <QPushButton>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#if defined(Q_OS_MACOS)
#include "MacHelper.h"

using namespace std::chrono_literals;

constexpr auto transisionDuration = 100ms;
constexpr auto toolbarItemWidth = 64;
constexpr auto alphaTransparent = 0;
constexpr auto alphaOpaque = 1;
#else
constexpr auto categoryFontAdjustment = 6;
constexpr auto settingsTreeWidth = 144;
constexpr auto settingsIconSize = 32;
constexpr auto settingsDialogScaleFactor = 0.5;
#endif

Nedrysoft::SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(nullptr)
{
#if defined(Q_OS_MACOS)
    m_currentPage = nullptr;

    m_toolBar = new QMacToolBar(this);

    m_animationGroup = nullptr;
#else
    resize((QSizeF(parent->frameSize())*settingsDialogScaleFactor).toSize());

    m_mainLayout = new QHBoxLayout;

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

    m_stackedWidget->layout()->setMargin(0);

    m_mainLayout->addWidget(m_treeWidget);

    m_categoryLabel = new QLabel;

    m_categoryLabel->setContentsMargins(4,0,0,9);

    m_categoryLabel->setFont(QFont(m_categoryLabel->font().family(), m_categoryLabel->font().pointSize()+categoryFontAdjustment));
    m_categoryLabel->setAlignment(Qt::AlignLeft);

    m_detailLayout = new QVBoxLayout;

    m_detailLayout->setContentsMargins(9,0,0,0);

    m_detailLayout->addWidget(m_categoryLabel);

    m_detailLayout->addWidget(m_stackedWidget);

    m_detailLayout->setAlignment(m_categoryLabel, Qt::AlignLeft);

    m_mainLayout->addLayout(m_detailLayout);

    m_mainLayout->setSpacing(0);

    m_layout = new QVBoxLayout;

    m_layout->addLayout(m_mainLayout);

    m_controlsLayout = new QHBoxLayout;

    m_controlsLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton("Cancel");
    m_applyButton = new QPushButton("Apply");

    m_controlsLayout->addWidget(m_okButton);
    m_controlsLayout->addWidget(m_cancelButton);
    m_controlsLayout->addWidget(m_applyButton);

    m_layout->addLayout(m_controlsLayout);

    setLayout(m_layout);
#endif
    addPage(tr("General"), tr("General settings"), SettingsPage::Icon::General, new GeneralSettingsPage, true);
    addPage(tr("Database"), tr("The database settings"), SettingsPage::Icon::Database, new DatabaseSettingsPage);

#if defined(Q_OS_MACOS)
    m_toolBar->attachToWindow(nativeWindowHandle());

    QSize size;

    for(auto page : m_pages) {
        size = QSize(qMax(page->m_widget->sizeHint().width(), size.width()), qMax(page->m_widget->sizeHint().height(), size.height()));
    }

    QPoint parentCentre(parent->frameGeometry().center());
    QPoint point((parentCentre.x()+size.width())/2, (parentCentre.y()+size.height())/2);

    move(point);

    m_toolbarHeight = frameGeometry().size().height()-geometry().size().height();

    if (m_currentPage) {
        auto minSize = QSize(qMax(toolbarItemWidth*m_pages.count(), m_currentPage->m_widget->sizeHint().width()), m_currentPage->m_widget->sizeHint().height());

        setMinimumSize(minSize);
        setMaximumSize(minSize);

        resize(minSize);
    }
#endif
}

Nedrysoft::SettingsDialog::~SettingsDialog()
{
#if defined(Q_OS_MACOS)
    m_toolBar->deleteLater();

    for(auto page : m_pages) {
        page->m_widget->deleteLater();

        delete page;
    }
#else
    m_layout->deleteLater();
    m_treeWidget->deleteLater();
    m_categoryLabel->deleteLater();
    m_stackedWidget->deleteLater();

    for(auto page : m_pages) {
        delete page;
    }
#endif
}

bool Nedrysoft::SettingsDialog::close()
{
#if defined(Q_OS_MACOS)
    for(auto page : m_pages) {
        page->m_pageSettings->acceptSettings();
    }
#endif
    return true;
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

    window()->winId();

    return window()->windowHandle();
}

Nedrysoft::SettingsPage *Nedrysoft::SettingsDialog::addPage(QString name, QString description, SettingsPage::Icon icon, QWidget *widget, bool defaultPage)
{
#if defined(Q_OS_MACOS)
    auto widgetContainer = new TransparentWidget(widget, 0, this);

    auto settingsPage = new SettingsPage;

    settingsPage->m_name = name;
    settingsPage->m_widget = widgetContainer;
    settingsPage->m_pageSettings = dynamic_cast<ISettingsPage *>(widget);
    settingsPage->m_icon = icon;
    settingsPage->m_description = description;

    if (widget->layout()) {
        widget->layout()->setSizeConstraint(QLayout::SetMinimumSize);
    }

    settingsPage->m_toolBarItem = m_toolBar->addItem(getIcon(icon), name);

    m_pages[settingsPage->m_toolBarItem] = settingsPage;

    connect(settingsPage->m_toolBarItem, &QMacToolBarItem::activated, this, [this, settingsPage]() {
        auto currentItem = m_pages[m_currentPage->m_toolBarItem]->m_widget;
        auto nextItem = settingsPage->m_widget;

        if (currentItem==nextItem) {
            return;
        }

        if (m_animationGroup) {
            m_animationGroup->stop();
            m_animationGroup->deleteLater();
        }

        m_animationGroup = new QParallelAnimationGroup;

        auto minSize = QSize(qMax(toolbarItemWidth*m_pages.count(), nextItem->sizeHint().width()), nextItem->sizeHint().height());

        auto propertyNames = {"size", "minimumSize", "maximumSize"};

        for(auto property : propertyNames) {
            auto sizeAnimation = new QPropertyAnimation(this, property);

            sizeAnimation->setDuration(transisionDuration.count());
            sizeAnimation->setStartValue(currentItem->size());
            sizeAnimation->setEndValue(minSize);

            m_animationGroup->addAnimation(sizeAnimation);
        }

        auto outgoingAnimation = new QPropertyAnimation(currentItem->transparencyEffect(), "opacity");

        outgoingAnimation->setDuration(transisionDuration.count());
        outgoingAnimation->setStartValue(currentItem->transparencyEffect()->opacity());
        outgoingAnimation->setEndValue(alphaTransparent);

        m_animationGroup->addAnimation(outgoingAnimation);

        auto incomingAnimation = new QPropertyAnimation(nextItem->transparencyEffect(), "opacity");

        incomingAnimation->setDuration(transisionDuration.count());
        incomingAnimation->setStartValue(nextItem->transparencyEffect()->opacity());
        incomingAnimation->setEndValue(alphaOpaque);

        m_animationGroup->addAnimation(incomingAnimation);

        m_animationGroup->start(QParallelAnimationGroup::DeleteWhenStopped);

        // the current page is set here immediately, so that if the page is changed again before the animation is
        // complete then the new selection will be animated in from the current position in the previous animation

        m_currentPage = settingsPage;

        connect(m_animationGroup, &QParallelAnimationGroup::finished, [this]() {
            m_animationGroup->deleteLater();

            m_animationGroup = 0;
        });
    });

    if (defaultPage) {
        if (m_currentPage) {
            m_currentPage->m_widget->setTransparency(0);
        }

        m_currentPage = settingsPage;

        m_currentPage->m_widget->setTransparency(1);

        resize(m_currentPage->m_widget->sizeHint());
    }

    return settingsPage;
#else
    auto newTreeItem = new QTreeWidgetItem(m_treeWidget);
    auto tabWidget = new QTabWidget();

    newTreeItem->setIcon(0, getIcon(icon));
    newTreeItem->setText(0, name);
    newTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(tabWidget));
    newTreeItem->setData(0, Qt::ToolTipRole, description);

    m_treeWidget->addTopLevelItem(newTreeItem);

    tabWidget->addTab(widget, "Interface");

    m_stackedWidget->addWidget(tabWidget);

    auto settingsPage = new SettingsPage;

    settingsPage->m_name = name;
    settingsPage->m_widget = widget;
    settingsPage->m_pageSettings = dynamic_cast<ISettingsPage *>(widget);
    settingsPage->m_icon = icon;
    settingsPage->m_description = description;

    connect(m_treeWidget, &QTreeWidget::currentItemChanged, [=](QTreeWidgetItem *current, [[maybe_unused]] QTreeWidgetItem *previous) {
        auto widget = current->data(0, Qt::UserRole).value<QWidget *>();

        if (widget) {
            m_stackedWidget->setCurrentWidget(widget);
            m_categoryLabel->setText(current->text(0));
        }
    });

    if (defaultPage) {
        m_treeWidget->setCurrentItem(newTreeItem);
    }

    return settingsPage;
#endif
}

QIcon Nedrysoft::SettingsDialog::getIcon(SettingsPage::Icon icon)
{
#if defined(Q_OS_MACOS)
    switch(icon) {
        case SettingsPage::Database: {
            return QIcon("://assets/Gianni-Polito-Colobrush-System-database.icns");
        }

        case SettingsPage::General: {
            return QIcon(Nedrysoft::MacHelper::macStandardImage(Nedrysoft::StandardImage::NSImageNamePreferencesGeneral, QSize(256,256)));
        }
    }
#else
    switch(icon) {
        case SettingsPage::Database: {
            return QIcon(":/assets/noun_database_2757856.png");
        }

        case SettingsPage::General: {
            return QIcon(":/assets/noun_Settings_716654.png");
        }
    }
#endif

    return QIcon();
}
