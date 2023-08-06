#include "richtexteditor.hpp"
#include "dbussession.hpp"
#include "tools.hpp"
#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <functional>
#include <vector>
#include <deque>
#include <unordered_map>
#include <iostream>

struct CLIApplication
{
    using Handler = std::function<void(std::string_view value)>;
    using Execution = std::function<void(QCommandLineParser const& parser)>;

    CLIApplication(QApplication& app);

    auto setApplicationName(std::string_view name) -> CLIApplication&;
    auto setApplicationDescription(std::string_view description) -> CLIApplication&;
    auto setApplicationVersion(std::string_view version) -> CLIApplication&;
    auto addOption(QCommandLineOption& option, bool required, Handler hndl) -> CLIApplication&;
    auto setupConflictedOptions(std::vector<std::reference_wrapper<QCommandLineOption>> const& conflictedOptions) -> CLIApplication&;
    auto setupExecutionAfterProcessing(Execution exec) -> CLIApplication&;

    auto process() -> void;

private:

    struct Option
    {
        QCommandLineOption& option;
        Handler handler;
        std::vector<std::string> conflicted;
        bool required;
    };

    std::vector<Option> m_options;
    QApplication& m_app;
    QCommandLineParser m_parser;
    Execution m_executeAfterProcessing;
};

auto main(int argc, char* argv[]) -> int
{
    try
    {
        QApplication app(argc, argv);

        CLIApplication cliApp{ app };

        QCommandLineOption detached{
                    QApplication::tr("detached"),
                    QApplication::tr("Run in detached mode. In this mode application unsinchronized with other.")
        };
        QCommandLineOption session{
                    QApplication::tr("session"),
                    QApplication::tr("Run application in specific session. Application will be synchronized with other apps in equals session."),
                    QApplication::tr("SESSION_NAME")
        };
        QCommandLineOption disabled{
            QApplication::tr("disabled"),
            QApplication::tr("Disable interaction between app instances througth DBus"),
        };

        cliApp.addOption(detached, false, [](auto value)
            {
                DBusSession::createDetached();
            }
        );

        cliApp.addOption(session, false, [](auto value)
            {
                DBusSession::createSession(value.data());
            }
        );

        cliApp.addOption(disabled, false, [](auto value)
            {
                DBusSession::createDisabled();
            }
        );

        cliApp.setupConflictedOptions(
            {
                std::ref(detached),
                std::ref(session),
                std::ref(disabled),
            }
        );

        cliApp.setupExecutionAfterProcessing([&](auto const& parser)
            {
                for (auto const& option : { detached, session, disabled })
                {
                    if (parser.isSet(option))
                    {
                        return;
                    }
                }

                DBusSession::createCommon();
            }
        );

        cliApp.process();

        RichTextEditor win;
        win.buildUi();
        win.show();

        return app.exec();
    }
    catch(std::exception const& excp)
    {
        std::cerr << excp.what() << std::endl;
    }
}

CLIApplication::CLIApplication(QApplication& app)
    : m_app{app}
    , m_parser{}
{
    m_parser.addHelpOption();
}

auto CLIApplication::setApplicationDescription(std::string_view description) -> CLIApplication&
{
    m_parser.setApplicationDescription(description.data());

    return *this;
}

auto CLIApplication::setApplicationName(std::string_view name) -> CLIApplication&
{
    m_app.setApplicationName(name.data());

    return *this;
}

auto CLIApplication::setApplicationVersion(std::string_view version) -> CLIApplication&
{
    m_app.setApplicationVersion(version.data());
    m_parser.addVersionOption();

    return *this;
}

auto CLIApplication::addOption(QCommandLineOption& option, bool required, Handler hndl) -> CLIApplication&
{
    auto opt = Option
    {
        option,
        hndl,
        std::vector<std::string>{},
        required,
    };

    m_options.emplace_back(std::move(opt));
    
    return *this;
}

struct QCommandLineOptionHasher
{
    size_t operator()(std::reference_wrapper<QCommandLineOption> val) const
    {
        return operator()(val.get());
    }

    size_t operator()(QCommandLineOption const& val) const
    {
        auto hasher = std::hash<std::string>{};
        std::size_t hsh{0};

        for (auto const& name : val.names())
        {
            hsh ^= hasher(name.toStdString()) << 1;
        }

        return hsh;
    }
};

struct QCommandLineOptionComparator
{
    bool operator()(std::reference_wrapper<QCommandLineOption> lhs, std::reference_wrapper<QCommandLineOption> rhs) const
    {
        return operator()(lhs.get(), rhs.get());
    }

    bool operator()(QCommandLineOption const& lhs, QCommandLineOption const& rhs) const
    {
        return lhs.names() == rhs.names();
    }
};

auto CLIApplication::setupConflictedOptions(std::vector<std::reference_wrapper<QCommandLineOption>> const& conflictedOptions) -> CLIApplication&
{
    std::unordered_multimap<
        std::reference_wrapper<QCommandLineOption>, std::string,
        QCommandLineOptionHasher, QCommandLineOptionComparator>
    conflicts;

    for (auto const& opt : conflictedOptions)
    {
        auto pos = std::find_if(std::begin(m_options), std::end(m_options), [&](Option const& inner_opt)
            {
                auto comparator = QCommandLineOptionComparator{};
                return comparator(inner_opt.option, opt.get());
            }
        );

        if (pos == std::end(m_options))
        {
            throw std::invalid_argument{"Option{" + opt.get().names().first().toStdString() + "} don't exist in inner options. You need add it before"};
        }

        for(auto const& oth_opt : conflictedOptions)
        {
            auto comparator = QCommandLineOptionComparator{};
            if (comparator(opt, oth_opt))
            {
                continue;
            }

            for (auto const& name : opt.get().names())
            {
                auto inner_name = name.toStdString();
                conflicts.insert(std::make_pair(oth_opt, inner_name));
            }
        }        
    }

    for (auto const& val : conflicts)
    {
        auto opt = val.first;
        auto name = val.second;

        auto inner_opt = std::find_if(std::begin(m_options), std::end(m_options), [&](Option const& inner_opt)
            {
                auto comparator = QCommandLineOptionComparator{};
                return comparator(inner_opt.option, opt.get());
            }
        );

        inner_opt->conflicted.emplace_back(std::move(name));
    }

    return *this;
}

auto CLIApplication::process() -> void
{
    std::vector<std::reference_wrapper<QCommandLineOption>> options;
    options.reserve(m_options.size());

    std::transform(std::begin(m_options), std::end(m_options), std::back_inserter(options), [](auto& option)
        {
            return std::ref(option.option);
        }
    );

    for (auto const& option : options)
    {
        m_parser.addOption(option.get());
    }

    m_parser.process(m_app);

    std::vector<std::reference_wrapper<Option>> exist_options;

    for (auto& inner_option: m_options)
    {
        if (!m_parser.isSet(inner_option.option))
        {
            continue;
        }

        for (auto const& name : inner_option.option.names())
        {
            auto inner_name = name.toStdString();
            auto pos = std::find_if(std::begin(exist_options), std::end(exist_options), [&](std::reference_wrapper<Option> option)
                {
                    auto ref = option.get();
                    auto count = std::count(std::begin(ref.conflicted), std::end(ref.conflicted), inner_name);
                    return count > 0;
                }
            );
            
            if (pos != std::end(exist_options))
            {
                std::cerr << QString{"Conflict option{%1} with option{%2}"}
                        .arg(name)
                        .arg(pos->get().option.names().first())
                        .toStdString()
                    << std::endl;

                m_parser.showHelp(EXIT_FAILURE);
            }

            exist_options.emplace_back(std::ref(inner_option));
        }
    }

    for (auto const& inner_opt : m_options)
    {
        if (m_parser.isSet(inner_opt.option))
        {
            auto fn = inner_opt.handler;
            fn(m_parser.value(inner_opt.option).toUtf8().data());
        }
    }

    m_executeAfterProcessing(m_parser);
}

auto CLIApplication::setupExecutionAfterProcessing(Execution exec) -> CLIApplication&
{
    m_executeAfterProcessing = exec;
    return *this;
}