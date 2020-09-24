#include "RegExAboutDialog.h"
#include "ui_RegExAboutDialog.h"
#include <QDebug>
#include <QDialog>
#include <QDesktopWidget>

RegExAboutDialog::RegExAboutDialog(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::RegExAboutDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
}

void RegExAboutDialog::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)

    close();
}

bool RegExAboutDialog::event(QEvent *event)
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

RegExAboutDialog::~RegExAboutDialog()
{
    delete ui;
}

