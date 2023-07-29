#include "formatactions.hpp"

#include "tools.hpp"

#define throwInvalidMemento(memento) \
	throw std::runtime_error{std::string{FUNC_SIGN} + std::string{" | Received invalid memento type: "} + typeName(memento.get())};

BoldMemento::BoldMemento(bool isBold)
	: OneItemMemento{isBold}
{	}

ActionType BoldMemento::getActionType() const
{
	return ActionType::FormatBold;
}

FormatBold::FormatBold(bool isBold, QTextEdit* textEditor)
	: TextEditorFormatAction{textEditor}
	, m_memento{std::make_unique<BoldMemento>(isBold)}
{	}

QTextCharFormat FormatBold::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontWeight(m_memento->m_item ? QFont::Bold : QFont::Normal);
	return fmt;
}

const Memento* FormatBold::getMemento() const
{
	return m_memento.get();
}

void FormatBold::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<BoldMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

}

ItalicMemento::ItalicMemento(bool isItalic)
	: OneItemMemento{isItalic}
{	}

ActionType ItalicMemento::getActionType() const
{
	return ActionType::FormatItalic;
}

FormatItalic::FormatItalic(bool isItalic, QTextEdit* textEditor)
	: TextEditorFormatAction{ textEditor }
	, m_memento{std::make_unique<ItalicMemento>(isItalic)}
{	}

QTextCharFormat FormatItalic::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontItalic(m_memento->m_item);
	return fmt;
}

const Memento* FormatItalic::getMemento() const
{
	return m_memento.get();
}

void FormatItalic::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<ItalicMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

UnderlineMemento::UnderlineMemento(bool isUnderline)
	: OneItemMemento{isUnderline}
{	}

ActionType UnderlineMemento::getActionType() const
{
	return ActionType::FormatUnderline;
}

FormatUnderline::FormatUnderline(bool isUnderline, QTextEdit* textEditor)
	: TextEditorFormatAction{textEditor}
	, m_memento{std::make_unique<UnderlineMemento>(isUnderline)}
{	}

QTextCharFormat FormatUnderline::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(m_memento->m_item);
	return fmt;
}

const Memento* FormatUnderline::getMemento() const
{
	return m_memento.get();
}

void FormatUnderline::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<UnderlineMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

ActionType AlignLeftMemento::getActionType() const
{
	return ActionType::FormatAlignLeft;
}

QTextCharFormat FormatAlignLeft::createCharFormat() const
{
	return QTextCharFormat{};
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

QTextCharFormat FormatAlignCenter::createCharFormat() const
{
	return QTextCharFormat{};
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

QTextCharFormat FormatAlignRight::createCharFormat() const
{
	return QTextCharFormat{};
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

QTextCharFormat FormatAlignJustify::createCharFormat() const
{
	return QTextCharFormat{};
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

FormatColor::FormatColor(QColor color, QTextEdit* textEditor)
	: TextEditorFormatAction{ textEditor }
	, m_memento{ std::make_unique<ColorMemento>(color) }
{	}

ColorMemento::ColorMemento(QColor color)
	: OneItemMemento{color}
{	}

ActionType ColorMemento::getActionType() const
{
	return ActionType::FormatColor;
}

QTextCharFormat FormatColor::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setForeground(m_memento->m_item);
	return fmt;
}

const Memento* FormatColor::getMemento() const
{
	return m_memento.get();
}

void FormatColor::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<ColorMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

UnderlineColorMemento::UnderlineColorMemento(QColor color)
	: OneItemMemento{color}
{	}

ActionType UnderlineColorMemento::getActionType() const
{
	return ActionType::FormatUnderlineColor;
}

FormatUnderlineColor::FormatUnderlineColor(QColor color, QTextEdit* textEditor)
	: TextEditorFormatAction{textEditor}
	, m_memento{std::make_unique<UnderlineColorMemento>(color)}
{	}


QTextCharFormat FormatUnderlineColor::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setUnderlineColor(m_memento->m_item);
	return fmt;
}

const Memento* FormatUnderlineColor::getMemento() const
{
	return m_memento.get();
}

void FormatUnderlineColor::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<UnderlineColorMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}

SizeMemento::SizeMemento(int size)
	: OneItemMemento{size}
{	}

ActionType SizeMemento::getActionType() const
{
	return ActionType::FontSize;
}

FormatSize::FormatSize(int size, QTextEdit* textEditor)
	: TextEditorFormatAction{textEditor}
	, m_memento{std::make_unique<SizeMemento>(size)}
{	}

QTextCharFormat FormatSize::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(m_memento->m_item);
	return fmt;
}

const Memento* FormatSize::getMemento() const
{
	return m_memento.get();
}

void FormatSize::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<SizeMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(memento);
}