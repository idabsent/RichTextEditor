#pragma once

#include "actions.hpp"

#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QMainWindow>

#include <functional>

using ExecuteAction = std::function<Action*(QAction*)>;

namespace
{
Action* defaultAction(QAction*)
{
    return nullptr;
}
}

struct MenuBarBuilder : public QObject
{
    MenuBarBuilder(QMainWindow* win);

    MenuBarBuilder* startBuild(QString const& barName, QString const& menuName);
    void endBuild();

    MenuBarBuilder* setActionPriority(QAction::Priority priority);
    MenuBarBuilder* setActionShortcut(QKeySequence shortcut);
    MenuBarBuilder* setActionIcon(QIcon&& icon);
    MenuBarBuilder* setCheckable(bool checkable);
    MenuBarBuilder* enableSepartorToMenu();
    MenuBarBuilder* disableForMenu();
    MenuBarBuilder* disableForToolBar();

    QAction* createAction(QString const& name, ExecuteAction fn = ::defaultAction);

private:
    void reset();

    QMainWindow* m_win;
    QIcon m_icon;
    QToolBar* m_toolBar;
    QMenu* m_menu;
    QKeySequence m_shortcut;
    QAction::Priority m_priority;
    bool m_enableSeparator;
    bool m_disableForMenu;
    bool m_disableForBar;
    bool m_checkable;
};
