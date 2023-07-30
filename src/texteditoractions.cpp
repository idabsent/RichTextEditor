#include "texteditoractions.hpp"

#include <exception>

#include <QDataStream>

std::unique_ptr<TextEditorFormatMementoBuilder> TextEditorFormatMementoBuilder::_instance;

TextEditorFormatMementoBuilder::TextEditorFormatMementoBuilder(QTextEdit* editor)
	: m_editor{editor}
{	}

bool TextEditorFormatMementoBuilder::supportAction(ActionType action) const
{
	switch (action)
	{
	case ActionType::FormatBold:
	case ActionType::FormatItalic:
	case ActionType::FormatUnderline:
	case ActionType::FormatAlignLeft:
	case ActionType::FormatAlignRight:
	case ActionType::FormatAlignCenter:
	case ActionType::FormatAlignJustify:
	case ActionType::FormatIndent:
	case ActionType::FormatUnindent:
	case ActionType::FormatColor:
	case ActionType::FormatUnderlineColor:
	case ActionType::FormatChecked:
		return true;
	default:
		return false;
	}
}

//TODO enable support other action mementos
MementoUP TextEditorFormatMementoBuilder::buildMemento(QByteArray&& data, ActionType action)
{
	MementoUP memento;

	switch (action)
	{
	case ActionType::FormatBold:
	case ActionType::FormatItalic:
	case ActionType::FormatUnderline:
	case ActionType::FormatAlignLeft:
	case ActionType::FormatAlignRight:
	case ActionType::FormatAlignCenter:
	case ActionType::FormatAlignJustify:
	case ActionType::FormatIndent:
	case ActionType::FormatUnindent:
	case ActionType::FormatColor:
	case ActionType::FormatUnderlineColor:
	case ActionType::FormatChecked:
		break;
	}

	return MementoUP{ nullptr };
}

ActionUP TextEditorFormatMementoBuilder::buildAction(MementoUP memento)
{
	return ActionUP{ nullptr };
}

void TextEditorFormatMementoBuilder::createInstance(QTextEdit* editor)
{
	_instance.reset(new TextEditorFormatMementoBuilder{ editor });
}

TextEditorFormatMementoBuilder* TextEditorFormatMementoBuilder::instance()
{
	if (!_instance)
	{
		throw std::logic_error{"Attemption get uninitializaed instance..."};
	}

	return _instance.get();
}

TextEditorFormatAction::TextEditorFormatAction(QTextEdit* textEditor)
	: m_editor{textEditor}
{	}

void TextEditorFormatAction::execute()
{
	mergeFormat(createCharFormat());
}

void TextEditorFormatAction::mergeFormat(QTextCharFormat const& fmt)
{
	auto cursor = m_editor->textCursor();

	if (!cursor.hasSelection())
	{
		cursor.select(QTextCursor::WordUnderCursor);
	}

	cursor.mergeCharFormat(fmt);
	m_editor->mergeCurrentCharFormat(fmt);
}

TextEditorFormatIndentMemento::TextEditorFormatIndentMemento(QTextCursor const& cursor, int indent)
	: Memento{}
	, m_pos{cursor.position()}
	, m_indent{indent}
	, m_action{m_pos > 0 ? ActionType::FormatIndent : ActionType::FormatUnindent }
{	}

TextEditorFormatIndentMemento::TextEditorFormatIndentMemento(int pos, int indent)
	: Memento{}
	, m_pos{ pos }
	, m_indent{ indent }
	, m_action{ m_pos > 0 ? ActionType::FormatIndent : ActionType::FormatUnindent }
{	}

ActionType TextEditorFormatIndentMemento::getActionType() const
{
	return m_action;
}

QByteArray TextEditorFormatIndentMemento::toRaw() const
{
	QByteArray raw;
	QDataStream rawStream{ &raw, QIODevice::WriteOnly };

	rawStream << m_pos << m_indent;

	return raw;
}

void TextEditorFormatIndentMemento::initFromRaw(QByteArray&& raw)
{
	QDataStream rawStream{ &raw, QIODevice::ReadOnly };

	rawStream >> m_pos >> m_indent;
}

TextEditorFormatIndentAction::TextEditorFormatIndentAction(int indent, QTextEdit* textEditor)
	: m_editor{textEditor}
	, m_memento{std::make_unique<TextEditorFormatIndentMemento>(textEditor->textCursor(), indent)}
{	}

//TODO
void TextEditorFormatIndentAction::execute()
{
	
}

const Memento* TextEditorFormatIndentAction::getMemento() const
{
	return m_memento.get();
}

void TextEditorFormatIndentAction::setMemento(MementoUP memento)
{
	auto memento_o = dynamic_cast<TextEditorFormatIndentMemento*>(memento.get());
	if (!memento_o)
	{
		auto mementoType = typeid(memento_o).name();
		throw std::runtime_error{"Received memento of other type: " + std::string{mementoType}};
	}

	m_memento = std::move(memento);
}

TextEditorFormatCheckedMemento::TextEditorFormatCheckedMemento(QTextCursor const& cursor, bool isChecked)
	: TextEditorFormatCheckedMemento{cursor.position(), isChecked}
{	}

TextEditorFormatCheckedMemento::TextEditorFormatCheckedMemento(int pos, bool isChecked)
	: m_pos{pos}
	, m_checked{isChecked}
{	}

ActionType TextEditorFormatCheckedMemento::getActionType() const
{
	return ActionType::FormatChecked;
}

QByteArray TextEditorFormatCheckedMemento::toRaw() const
{
	QByteArray raw;
	QDataStream rawStream{ &raw, QIODevice::WriteOnly };

	rawStream << m_pos << m_checked;

	return raw;
}

void TextEditorFormatCheckedMemento::initFromRaw(QByteArray&& raw)
{
	QDataStream rawStream{ &raw, QIODevice::WriteOnly };

	rawStream >> m_pos >> m_checked;
}

TextEditorFormatCheckedAction::TextEditorFormatCheckedAction(bool checked, QTextEdit* textEditor)
	: m_editor{ textEditor }
	, m_memento{ std::make_unique< TextEditorFormatCheckedMemento>(textEditor->textCursor(), checked) }
{	}

//TODO implement action
void TextEditorFormatCheckedAction::execute()
{
	
}

const Memento* TextEditorFormatCheckedAction::getMemento() const
{
	return m_memento.get();
}

void TextEditorFormatCheckedAction::setMemento(MementoUP memento)
{
	m_memento = std::move(memento);
}


ActionType AlignLeftMemento::getActionType() const
{
	return ActionType::FormatAlignLeft;
}

//TODO
void FormatAlignLeft::execute()
{

}

const Memento* FormatAlignLeft::getMemento() const
{
	return m_memento.get();
}

void FormatAlignLeft::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<AlignLeftMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

ActionType AlignCenterMemento::getActionType() const
{
	return ActionType::FormatAlignCenter;
}

//TODO
void FormatAlignCenter::execute() 
{

}

const Memento* FormatAlignCenter::getMemento() const
{
	return m_memento.get();
}

void FormatAlignCenter::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<AlignCenterMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

ActionType AlignRightMemento::getActionType() const
{
	return ActionType::FormatAlignRight;
}

//TODO
void FormatAlignRight::execute()
{

}

const Memento* FormatAlignRight::getMemento() const
{
	return m_memento.get();
}

void FormatAlignRight::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<AlignRightMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

ActionType AlignJustifyMemento::getActionType() const
{
	return ActionType::FormatAlignJustify;
}

//TODO
void FormatAlignJustify::execute()
{

}

const Memento* FormatAlignJustify::getMemento() const
{
	return m_memento.get();
}

void FormatAlignJustify::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<AlignJustifyMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}