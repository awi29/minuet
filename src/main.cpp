/****************************************************************************
**
** Copyright (C) 2015 by Sandro S. Andrade <sandroandrade@kde.org>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of
** the License or (at your option) version 3 or any later version
** accepted by the membership of KDE e.V. (or its successor approved
** by the membership of KDE e.V.), which shall act as a proxy 
** defined in Section 14 of version 3 of the license.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "minuet.h"

#include <KAboutData>
#include <KLocalizedString>

#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>

Q_DECLARE_LOGGING_CATEGORY(MINUET)
Q_LOGGING_CATEGORY(MINUET, "minuet")

int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("minuet");
    KAboutData aboutData( QStringLiteral("minuet"),
                          i18n("Minuet"),
                          QStringLiteral("0.1"),
                          i18n("A KDE application for music education"),
                          KAboutLicense::GPL,
                          i18n("(c) 2015, Sandro S. Andrade <sandroandrade@kde.org>"));

    aboutData.addAuthor(i18n("Sandro S. Andrade"),i18n("Author"), QStringLiteral("sandroandrade@kde.org"));
    application.setWindowIcon(QIcon::fromTheme("minuet"));
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(application);
    aboutData.processCommandLine(&parser);
    KAboutData::setApplicationData(aboutData);

    Minuet *appwindow = new Minuet;
    appwindow->show();
    return application.exec();
}