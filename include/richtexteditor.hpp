#pragma once

#include <QMainWindow>
#include <QTextEdit>

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

    void buildEditor();

    QTextEdit* m_textEditor;
};
