#include "editortabwidget.hpp"

#include <QLayout>

EditorTabWidget::EditorTabWidget(QTextEdit* textEditor, QWidget* parent)
    : QWidget{parent}
    , m_textEditor{textEditor}
    , m_tabs{new QTabBar}
{
    connect(m_tabs, &QTabBar::currentChanged, this, &EditorTabWidget::onCurrentChanged);

    auto layout = new QVBoxLayout{this};
    layout->addWidget(m_tabs);
    layout->addWidget(m_textEditor);
    m_textEditor->setDisabled(true);
}

auto EditorTabWidget::addDocument(QString const& title, QTextDocument* doc, bool overwrite) -> void
{
    m_textEditor->setEnabled(true);
    auto oldDoc = m_docs.value(title);

    if (oldDoc && overwrite)
    {
        delete doc;
    }
    else if (oldDoc)
    {
        return;
    }

    m_docs.insert(title, doc);
    m_tabs->addTab(title);
    m_textEditor->setDocument(doc);
    m_textEditor->setDocumentTitle(title);
    m_tabs->setCurrentIndex(m_tabs->count() - 1);

}

auto EditorTabWidget::changeCurrentTitle(QString const& newTitle) -> void
{
    auto pos = m_tabs->currentIndex();
    auto title = m_tabs->tabText(pos);
    auto doc = m_docs.value(title);

    m_docs.remove(title);
    m_docs.insert(newTitle, doc);
    m_tabs->setTabText(pos, newTitle);
    m_textEditor->setDocumentTitle(newTitle);
}

auto EditorTabWidget::getCurrentDocument() const -> QTextDocument*
{
    return m_textEditor->document();
}

auto EditorTabWidget::getEditor() const -> QTextEdit*
{
    return m_textEditor;
}

void EditorTabWidget::onCurrentChanged(int index)
{
    auto title = m_tabs->tabText(index);
    auto doc = m_docs.value(title);

    m_textEditor->setDocument(doc);
    m_textEditor->setDocumentTitle(title);
}
