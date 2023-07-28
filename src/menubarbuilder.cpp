#include "menubarbuilder.hpp"

#include <QMenuBar>

MenuBarBuilder::MenuBarBuilder(QMainWindow* win, QString const& barName, QString const& menuName)
    : m_win{win}
{
    m_toolBar = m_win->addToolBar(barName);
    m_menu = m_win->menuBar()->addMenu(menuName);

    reset();
}

MenuBarBuilder& MenuBarBuilder::setActionPriority(QAction::Priority priority)
{
    m_priority = priority;
    return *this;
}

MenuBarBuilder& MenuBarBuilder::setActionShortcut(QKeySequence shortcut)
{
    m_shortcut = shortcut;
    return *this;
}

MenuBarBuilder& MenuBarBuilder::setActionIcon(QIcon&& icon)
{
    m_icon = std::move(icon);
    return *this;
}

MenuBarBuilder& MenuBarBuilder::enableSepartorToMenu()
{
    m_enableSeparator = true;
    return *this;
}

MenuBarBuilder& MenuBarBuilder::disableForMenu()
{
    m_disableForMenu = true;
    return *this;
}

MenuBarBuilder& MenuBarBuilder::disableForToolBar()
{
    m_disableForBar = true;
    return *this;
}

QAction* MenuBarBuilder::createAction(QString const& name)
{
    auto action = new QAction{name};
    action->setShortcut(m_shortcut);
    action->setPriority(m_priority);

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
}
