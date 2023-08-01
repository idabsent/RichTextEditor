#include "menubarbuilder.hpp"
#include "tools.hpp"
#include "dbussession.hpp"

#include <QMenuBar>
#include <QDebug>

MenuBarBuilder::MenuBarBuilder(QMainWindow* win)
    : QObject{win}
    , m_win{win}
    , m_toolBar{nullptr}
    , m_menu{nullptr}
{
    reset();
}

MenuBarBuilder* MenuBarBuilder::startBuild(QString const& barName, QString const& menuName)
{
    m_toolBar = new QToolBar{barName};
    m_menu = new QMenu{menuName};

    return this;
}

void MenuBarBuilder::endBuild()
{
    m_win->addToolBar(m_toolBar);
    m_win->menuBar()->addMenu(m_menu);

    m_toolBar = nullptr;
    m_menu = nullptr;
}

MenuBarBuilder* MenuBarBuilder::setActionPriority(QAction::Priority priority)
{
    m_priority = priority;
    return this;
}

MenuBarBuilder* MenuBarBuilder::setActionShortcut(QKeySequence shortcut)
{
    m_shortcut = shortcut;
    return this;
}

MenuBarBuilder* MenuBarBuilder::setActionIcon(QIcon&& icon)
{
    m_icon = std::move(icon);
    return this;
}

MenuBarBuilder* MenuBarBuilder::enableSepartorToMenu()
{
    m_enableSeparator = true;
    return this;
}

MenuBarBuilder* MenuBarBuilder::disableForMenu()
{
    m_disableForMenu = true;
    return this;
}

MenuBarBuilder* MenuBarBuilder::disableForToolBar()
{
    m_disableForBar = true;
    return this;
}

MenuBarBuilder* MenuBarBuilder::setCheckable(bool checkable)
{
    m_checkable = true;
    return this;
}

QAction* MenuBarBuilder::createAction(QString const& name, ExecuteAction fn)
{
    auto action = new QAction{name};
    action->setShortcut(m_shortcut);
    action->setPriority(m_priority);
    action->setCheckable(m_checkable);

    QObject::connect(action, &QAction::triggered, m_win, [=]
    {
        qDebug() << FUNC_SIGN << ": action" << name << "triggered";
        if (auto actionObject = fn(action))
        {
            actionObject->execute();
            std::unique_ptr<Action> action{actionObject};
            DBusSession::instance()->sendAction(std::move(action));
        }
    });

    if (!m_icon.isNull())
    {
        action->setIcon(m_icon);
    }

    if (m_enableSeparator)
    {
        m_menu->addSeparator();
    }

    if (!m_disableForBar)
    {
        m_toolBar->addAction(action);
    }

    if (!m_disableForMenu)
    {
        m_menu->addAction(action);
    }

    reset();

    return action;
}

void MenuBarBuilder::reset()
{
    m_icon = QIcon{};
    m_enableSeparator = false;
    m_shortcut = QKeySequence::UnknownKey;
    m_priority = QAction::NormalPriority;
    m_disableForBar = false;
    m_disableForMenu = false;
    m_checkable = false;
}
