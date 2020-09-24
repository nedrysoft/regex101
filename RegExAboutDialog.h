#ifndef REGEXABOUTDIALOG_H
#define REGEXABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class RegExAboutDialog;
}

class RegExAboutDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit RegExAboutDialog(QWidget *parent = nullptr);
        ~RegExAboutDialog();

    protected:
        virtual bool event(QEvent *event);
        virtual void focusOutEvent(QFocusEvent *event);

    private:
        Ui::RegExAboutDialog *ui;
};

#endif // REGEXABOUTDIALOG_H
