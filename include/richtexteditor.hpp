#pragma once

#include <QMainWindow>
#include <QTextEdit>

#include "menubarbuilder.hpp"
#include "editorobservers.hpp"

struct RichTextEditor : public QMainWindow
{
    RichTextEditor(QMainWindow* parent = nullptr);

    void buildUi();

private:
    Q_OBJECT

    void setupFileActions();
    void setupFormatActions();
    void setupEditActions();
    void setupFontSelectorToolBar();

    void buildEditorAndObjects();

    QTextEdit* m_textEditor;
    MenuBarBuilder* m_builder;
    TextChangeObserver* m_textObserver;
    DBusActionsObserver* m_actionsObserver;
};
