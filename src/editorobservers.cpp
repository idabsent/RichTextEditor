#include "editorobservers.hpp"
#include "tools.hpp"
#include "dbussession.hpp"
#include "texteditoractions.hpp"

#include <QDebug>
#include <QKeyEvent>

QChar AdditionalEmiterTextEditor::nextChar()
{
	auto cursor = textCursor();
	cursor.selectionStart();
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
	cursor.selectionEnd();
	return cursor.selectedText()[0];
}

QChar AdditionalEmiterTextEditor::previousChar()
{
	auto cursor = textCursor();
	cursor.selectionStart();
	cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	cursor.selectionEnd();
	return cursor.selectedText().at(0);
}

void AdditionalEmiterTextEditor::keyReleaseEvent(QKeyEvent* event)
{
	if (!event->text().isEmpty() && event->text().front().isPrint())
	{
		emit symAdded(previousChar());
	}

	QTextEdit::keyReleaseEvent(event);
}

void AdditionalEmiterTextEditor::keyPressEvent(QKeyEvent* event)
{
	event->accept();

	auto key = event->key();
	auto cursor = textCursor();

	if (key == Qt::Key_Backspace && !cursor.atStart())
	{
		emit symRemoved(previousChar());
	}
	else if (key == Qt::Key_Delete && !cursor.atEnd())
	{
		emit symRemoved(nextChar());
	}

	QTextEdit::keyPressEvent(event);
}

TextChangeObserver::TextChangeObserver(AdditionalEmiterTextEditor* textEditor, size_t buffSize)
	: QObject{textEditor}
	, m_editor{textEditor}
{
	connect(m_editor, &AdditionalEmiterTextEditor::symAdded, this, &TextChangeObserver::onSymAdded);
	connect(m_editor, &AdditionalEmiterTextEditor::symRemoved, this, &TextChangeObserver::onSymRemoved);
}

void TextChangeObserver::onSymAdded(QChar const& sym)
{
	auto dbusSession = DBusSession::instance();
	auto action = std::unique_ptr<Action>{ 
		new TextChangeAction{ TextChangeType::Added, sym, m_editor } 
	};
	dbusSession->sendAction(std::move(action));
}

void TextChangeObserver::onSymRemoved(QChar const& sym)
{
	auto dbusSession = DBusSession::instance();
	auto action = std::unique_ptr<Action>{
		new TextChangeAction{TextChangeType::Removed, sym, m_editor}
	};
	dbusSession->sendAction(std::move(action));
}

DBusActionsObserver::DBusActionsObserver(QTextEdit* textEditor, QObject* parent)
	: QObject{parent}
	, m_editor{textEditor}
{
	connect(DBusSession::instance(), &DBusSession::actionReceived, this, &DBusActionsObserver::onDataReceived);
}

void DBusActionsObserver::onDataReceived(ActionType type, QByteArray const& raw)
{
	auto data = raw;
    /*
    for (auto builder : GLOBAL_BUILDERS)
	{
		if (builder->supportAction(type))
		{
			auto memento = builder->buildMemento(std::move(data), type);
			auto action = builder->buildAction(std::move(memento));
			action->execute();
		}
	}
    */

    auto builder = GlobalMementoBuilder::instance();
    if (builder->supportAction(type))
    {
        auto memento = builder->buildMemento(std::move(data), type);
        auto action = builder->buildAction(std::move(memento));
        action->execute();
    }
}
