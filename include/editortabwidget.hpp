#pragma once

#include <QWidget>
#include <QTabBar>
#include <QHash>
#include <QTextEdit>

struct EditorTabWidget : public QWidget
{
    EditorTabWidget(QTextEdit* textEditor, QWidget* parent = nullptr);

    auto addDocument(QString const& title, QTextDocument* doc, bool overwrite = false) -> void;
    auto changeCurrentTitle(QString const& newTitle) -> void;
    auto getCurrentDocument() const -> QTextDocument*;
    auto getEditor() const -> QTextEdit*;

signals:
    void currentDocumentChanged(QTextDocument* doc);

private slots:
    void onCurrentChanged(int index);

private:
    Q_OBJECT

    QTextEdit* m_textEditor;
    QTabBar* m_tabs;
    QHash<QString, QTextDocument*> m_docs;
};