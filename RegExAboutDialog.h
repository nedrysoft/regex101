#ifndef REGEXABOUTDIALOG_H
#define REGEXABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class RegExAboutDialog;
}

namespace Nedrysoft {
    /**
     * @brief           RegExAboutDialog class
     *
     * @details         A dialog box that shows details about the application.
     */
    class RegExAboutDialog : public QDialog {
        Q_OBJECT

        public:
            /**
             * @brief       Constructs an about dialog.
             *
             * @param[in]   parent is the the owner of the dialog.
             */
            explicit RegExAboutDialog(QWidget *parent = nullptr);

            /**
             * @brief       Destroys the dialog.
             */
            ~RegExAboutDialog();

        protected:
            /**
             * @brief       Handles events for the dialog box.
             *
             * @details     To allow the dialog box to be dismissed easily, event processing at a more
             *              granular level needs to occur.
             *
             * @param[in]   event contains information about the event being processed.
             * @returns     true if the event was handled; otherwise, false.
             */
            virtual bool event(QEvent *event) override;

            /**
             * @brief       Handles focus out events.
             *
             * @details     If the about dialog loses focus, this function is called to dismiss the dialog.
             *
             * @param[in]   event contains information about the focus event.
             */
            virtual void focusOutEvent(QFocusEvent *event) override;

        private:
            Ui::RegExAboutDialog *ui;
    };
}
#endif // REGEXABOUTDIALOG_H
