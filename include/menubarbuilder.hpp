#pragma once

#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QMainWindow>

struct MenuBarBuilder
{
    MenuBarBuilder(QMainWindow* win, QString const& barName, QString const& menuName);

    MenuBarBuilder& setActionPriority(QAction::Priority priority);
    MenuBarBuilder& setActionShortcut(QKeySequence shortcut);
    MenuBarBuilder& setActionIcon(QIcon&& icon);
    MenuBarBuilder& enableSepartorToMenu();
    MenuBarBuilder& disableForMenu();
    MenuBarBuilder& disableForToolBar(); 

    QAction* createAction(QString const& name);

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
};
