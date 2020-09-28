#include "RegExAboutDialog.h"
#include "ui_RegExAboutDialog.h"

#include <QDebug>
#include <QDialog>
#include <QDesktopWidget>

Nedrysoft::RegExAboutDialog::RegExAboutDialog(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::RegExAboutDialog)
{
    ui->setupUi(this);

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

    return QObject::event(event);
}

Nedrysoft::RegExAboutDialog::~RegExAboutDialog()
{
    delete ui;
}

