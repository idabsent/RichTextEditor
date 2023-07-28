#pragma once

#include <QMainWindow>
#include <QTextEdit>

struct RichText : public QMainWindow
{
    RichText(QMainWindow* parent = nullptr);

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
