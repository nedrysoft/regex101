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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace Nedrysoft {
    class RegExSplashScreen;
    class RegExWebEnginePage;

    /**
     * @brief           The MainWindow class
     *
     * @details         Provides the main window for the application.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        public:
            /**
             * @brief       Constructs the main window.
             *
             * @param[in]   splashScreen is a pointer to the splashscreen that was created
             *              by the main thread at startup.
             * @param[in]   parent is the parent widget that owns this main window.
             */
            MainWindow(Nedrysoft::RegExSplashScreen *splashScreen, QWidget *parent = nullptr);

            /**
             * @brief       Destroys the main window.
             */
            ~MainWindow();

        private slots:
            /**
             * @brief       About slot function.
             *
             * @details     This slot is called when the About action is triggered, the about dialog is displayed.
             */
            void on_actionAbout_triggered();

            /**
             * @brief       Exit slot function.
             *
             * @details     This slot is called when the Exit action is triggered, the application is closed.
             */
            void on_actionExit_triggered();

        private:
            Ui::MainWindow *ui;
            Nedrysoft::RegExWebEnginePage *m_page;
    };
}

#endif // MAINWINDOW_H
