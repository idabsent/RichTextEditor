#pragma once

#include <QMainWindow>
#include <QTextEdit>

#include "menubarbuilder.hpp"
#include "editorobservers.hpp"
#include "editortabwidget.hpp"

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

    EditorTabWidget* m_docsEditor;
    MenuBarBuilder* m_builder;
    TextChangeObserver* m_textObserver;
    DBusActionsObserver* m_actionsObserver;
};
