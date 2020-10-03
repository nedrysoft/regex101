#include "RegExAboutDialog.h"
#include "ui_RegExAboutDialog.h"

#include <QDebug>
#include <QDialog>
#include <QDesktopWidget>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QString>

constexpr auto splashScreenFilename = ":/assets/splash_620x375@2x.png";
constexpr auto fontFamily = "Open Sans";
constexpr auto fontSize = 14;

Nedrysoft::RegExAboutDialog::RegExAboutDialog(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint)
{
    m_backgroundPixmap = QPixmap(splashScreenFilename);

    auto dialogSize = (QSizeF(m_backgroundPixmap.size())*(devicePixelRatioF()/m_backgroundPixmap.devicePixelRatioF())).toSize();

    resize(dialogSize);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
}

void Nedrysoft::RegExAboutDialog::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)

    close();
}

bool Nedrysoft::RegExAboutDialog::event(QEvent *event)
{
    switch(event->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::KeyPress: {
            close();
            break;
        }

        default: {
            break;
        }
    }

    return QDialog::event(event);
}

Nedrysoft::RegExAboutDialog::~RegExAboutDialog()
{
}

void Nedrysoft::RegExAboutDialog::paintEvent(QPaintEvent *paintEvent)
{
    QDialog::paintEvent(paintEvent);

    auto font = QFont(fontFamily, fontSize, QFont::Weight::Normal);
    auto versionText = QString("%1.%2.%3 (%4 %5)").arg(APPLICATION_GIT_YEAR).arg(APPLICATION_GIT_MONTH).arg(APPLICATION_GIT_DAY).arg(APPLICATION_GIT_BRANCH).arg(APPLICATION_GIT_HASH);

    QPainter painter(this);

    painter.save();

    painter.drawPixmap(0, 0, m_backgroundPixmap);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.setFont(font);

    painter.drawText(QRect(350, 300, 250, 71), Qt::AlignRight | Qt::AlignVCenter, versionText);

    painter.restore();
}
