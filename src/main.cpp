#include "richtexteditor.hpp"

#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

auto main(int argc, char* argv[]) -> int
{
    QApplication app(argc, argv);
    app.setApplicationName(PROJECT_NAME);
    app.setApplicationVersion(PROJECT_VERSION);

    QCommandLineParser cliParser;

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

    RichTextEditor win;
    win.buildUi();
    win.show();

    return app.exec();
}
