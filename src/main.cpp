/*
*  main.cpp
*  QUIMUP main program file
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include <QTranslator>
#include <QLocale>
#include <QTextCodec>
#include <QDir>
#include "qtsingleapplication.h"
#include "qm_core.h"


int main(int argc, char **argv)
{
    QtSingleApplication app("quimup", argc, argv);
    bool b_multiple_instances = false;
    QString locale = "xx";
    QString message;
    int filecount = 0;
    // handle command line args
    for (int a = 1; a < argc; ++a)
    {
        QString msg = QString::fromUtf8(argv[a]);

        if ( msg == "-p" || msg == "-play")
        {
            message = "-play:" + message;
        }
        else
        if ( msg == "-i" || msg == "-instance")
        {
            b_multiple_instances = true;
        }
        else
        if ( msg == "-l"  || msg == "-locale" )
        {
            if (a < (argc-1) )
            {
                locale = argv[a+1];
                // possible stray '-l'
                if ( locale.startsWith("-") || locale.startsWith("file") )
                    locale = "xx";
                else
                    a++;
            }
        }
        else
        if (msg == "-h" || msg == "-help")
        {
            printf ("----\nQuimup version 1.3.1\n"); // VERSION //
            printf ("© 2008-2013 Johan Spee <quimup@coonsden.com>\n");
            printf ("This program is licensed by the GPL and distributed in the hope that it will be useful, but without any warranty.\n");
            printf ("----\ncommand line parameters:\n");
            printf (" -h(elp)           show this information and exit\n");
            printf (" -i(nstance)       force a new instance of Quimup\n");
            printf (" -l(ocale) xx      use locale xx (fr, po, en_GB, etc)\n");
            printf (" -p(lay) %%U        play files in %%U (or %%F) in new playlist\n");
            printf ("  %%U               append files in %%U (or %%F) to the playlist\n----\n");
            return 0;
        }
        else
        // if URL
        if ( msg.startsWith("file://"))
        {
            // we have a string in URL format, so convert special chars:
            msg = ( QUrl::fromEncoded ((const char*)msg.toUtf8()) ).toString();
            message += msg;
            filecount++;
        }
        else
        //  if plain path
        if (msg.startsWith("/") )
        {
            msg = "file://" + msg; // used as separator
            message += msg;
            filecount++;
        }
    }

    // send the args (and find out if a previous instance exists)
    if (!b_multiple_instances && app.sendMessage(message))
    {
        printf ("Quimup : already running (use '-i' to force a new instance)\n");
        return 0;
    }

    printf ("Quimup : starting new instance\n");

    //// Localization
    QTranslator trnsltr;
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    bool b_locale_default = true;
    if (locale == "xx")
        locale = QLocale::system().name();
    else
        b_locale_default = false;

    if (locale != "xx")
    {
        if (b_locale_default)
            printf ("Locale : ");
        else
            printf ("Locale : '%s': ", (const char*)locale.toUtf8());

        if ( locale.startsWith("/") && locale.endsWith(".qm")) // full path to a file
        {
            if ( trnsltr.load(locale) )
            {
                if (b_locale_default)
                    printf ("using default\n");
                else
                    printf ("OK\n");
            }
            else
            {
                if (b_locale_default)
                    printf ("using none\n");
                else
                    printf ("failed\n");
            }
        }
        else
        {
            if ( !trnsltr.load(QString("quimup_") + locale, "/usr/share/quimup/translations/") )
            {
                QDir dir(QApplication::applicationDirPath());
                if ( !trnsltr.load(QString("quimup_") + locale, dir.absolutePath()) )
                {
                    if (b_locale_default)
                        printf ("using none\n");
                    else
                        printf ("failed\n");
                }
                else
                {
                    if (b_locale_default)
                        printf ("using default\n");
                    else
                        printf ("found in %s\n", (const char*)dir.absolutePath().toUtf8());
                }
            }
            else
                printf ("found in /usr/share/quimup/translations\n");
        }
    }

    app.installTranslator(&trnsltr);


    qm_core *core = new qm_core();

    // handle files
    if (filecount > 0)
    {
        core->player->browser_window->plist_view->on_open_with_request(message);
    }

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)), core, SLOT(on_message_from_2nd_instance(const QString&)));

    return app.exec();
}
