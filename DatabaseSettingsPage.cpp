#include "DatabaseSettingsPage.h"
#include "ui_DatabaseSettingsPage.h"

#include <QDebug>

Nedrysoft::DatabaseSettingsPage::DatabaseSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseSettingsPage)
{
    ui->setupUi(this);

#if defined(Q_OS_MACOS)
    m_size = QSize(qMax(minimumSizeHint().width(), size().width()), qMax(minimumSizeHint().height(), size().height()));
#else
    m_size = minimumSizeHint();
#endif
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

}

QSize Nedrysoft::DatabaseSettingsPage::sizeHint() const
{
    return m_size;
}
