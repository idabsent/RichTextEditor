#include "richtexteditor.hpp"
#include "dbussession.hpp"
#include "tools.hpp"

#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <random>

auto main(int argc, char* argv[]) -> int
{
    QApplication app(argc, argv);
    app.setApplicationName(PROJECT_NAME);
    app.setApplicationVersion(PROJECT_VERSION);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(PROJECT_DESCRIPTION);

    QCommandLineOption detached{
                QApplication::tr("detached"),
                QApplication::tr("Run in detached mode. In this mode application unsinchronized with other.")
    };
    QCommandLineOption session{
                QApplication::tr("session"),
                QApplication::tr("Run application in specific session. Application will be synchronized with other apps in equals session."),
                QApplication::tr("SESSION_NAME")
    };

    cliParser.addOption(detached);
    cliParser.addOption(session);
    cliParser.addHelpOption();
    cliParser.addVersionOption();

    cliParser.process(app);

    if (cliParser.isSet(detached) && cliParser.isSet(session))
    {
        cliParser.showHelp(EXIT_FAILURE);
    }

    if (cliParser.isSet(detached))
    {
        auto randomSession = QString::number(tools::generate_random(0, 1000));
        DBusSession::createInstance("session_" + randomSession);
    }
    else if (cliParser.isSet(session))
    {
        DBusSession::createInstance(cliParser.value(session));
    }
    else
    {
        DBusSession::createInstance("common");
    }

    RichTextEditor win;
    win.buildUi();
    win.show();

    return app.exec();
}
