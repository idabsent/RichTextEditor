#include "formatactions.hpp"

#include "tools.hpp"

#include <QDebug>

BoldMemento::BoldMemento(QTextCursor const& cursor, bool isBold)
	: StreamItemsMemento{ cursor.position(), isBold }
{	}

ActionType BoldMemento::getActionType() const
{
	return ActionType::FormatBold;
}

FormatBold::FormatBold(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatBold::FormatBold(bool isBold, QTextEdit* textEditor)
    : FormatAction{textEditor}
	, m_memento{std::make_unique<BoldMemento>(textEditor->textCursor(), isBold)}
{	}

int FormatBold::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatBold::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontWeight(std::get<1>(m_memento->m_items) ? QFont::Bold : QFont::Normal);
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

ItalicMemento::ItalicMemento(QTextCursor const& cursor, bool isItalic)
	: StreamItemsMemento{cursor.position(), isItalic}
{	}

ActionType ItalicMemento::getActionType() const
{
	return ActionType::FormatItalic;
}

FormatItalic::FormatItalic(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatItalic::FormatItalic(bool isItalic, QTextEdit* textEditor)
    : FormatAction{ textEditor }
	, m_memento{std::make_unique<ItalicMemento>(textEditor->textCursor(), isItalic)}
{	}

int FormatItalic::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatItalic::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontItalic(std::get<1>(m_memento->m_items));
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

UnderlineMemento::UnderlineMemento(QTextCursor const& cursor, bool isUnderline)
	: StreamItemsMemento{cursor.position(), isUnderline}
{	}

ActionType UnderlineMemento::getActionType() const
{
	return ActionType::FormatUnderline;
}

FormatUnderline::FormatUnderline(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatUnderline::FormatUnderline(bool isUnderline, QTextEdit* textEditor)
    : FormatAction{textEditor}
	, m_memento{std::make_unique<UnderlineMemento>(textEditor->textCursor(), isUnderline)}
{	}

int FormatUnderline::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatUnderline::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(std::get<1>(m_memento->m_items));
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

FormatColor::FormatColor(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatColor::FormatColor(QColor color, QTextEdit* textEditor)
    : FormatAction{ textEditor }
	, m_memento{ std::make_unique<ColorMemento>(textEditor->textCursor(), color) }
{	}

ColorMemento::ColorMemento(QTextCursor const& cursor, QColor color)
	: StreamItemsMemento{cursor.position(), color}
{	}

ActionType ColorMemento::getActionType() const
{
	return ActionType::FormatColor;
}

int FormatColor::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatColor::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setForeground(std::get<1>(m_memento->m_items));
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

UnderlineColorMemento::UnderlineColorMemento(QTextCursor const& cursor, QColor color)
	: StreamItemsMemento{cursor.position(), color}
{	}

ActionType UnderlineColorMemento::getActionType() const
{
	return ActionType::FormatUnderlineColor;
}

FormatUnderlineColor::FormatUnderlineColor(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatUnderlineColor::FormatUnderlineColor(QColor color, QTextEdit* textEditor)
    : FormatAction{textEditor}
	, m_memento{std::make_unique<UnderlineColorMemento>(textEditor->textCursor(), color)}
{	}

int FormatUnderlineColor::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatUnderlineColor::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setUnderlineColor(std::get<1>(m_memento->m_items));
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

SizeMemento::SizeMemento(QTextCursor const& cursor, int size)
	: StreamItemsMemento{cursor.position(), size}
{	}

ActionType SizeMemento::getActionType() const
{
	return ActionType::FontSize;
}

FormatSize::FormatSize(QTextEdit* textEditor)
    : FormatAction{textEditor}
    , m_memento{nullptr}
{   }

FormatSize::FormatSize(int size, QTextEdit* textEditor)
    : FormatAction{textEditor}
	, m_memento{std::make_unique<SizeMemento>(textEditor->textCursor(), size)}
{	}

int FormatSize::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatSize::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(std::get<1>(m_memento->m_items));
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

FamilyMemento::FamilyMemento(QTextCursor const& cursor, QString const& family)
	: StreamItemsMemento{cursor.position(), family}
{	}

ActionType FamilyMemento::getActionType() const
{
	return ActionType::FontFamily;
}

FormatFamily::FormatFamily(QTextEdit* textEditor)
	: FormatAction{textEditor}
	, m_memento{nullptr}
{	}

FormatFamily::FormatFamily(QString const& family, QTextEdit* textEditor)
	: FormatAction{textEditor}
	, m_memento{ std::make_unique<FamilyMemento>(textEditor->textCursor(), family) }
{	}

int FormatFamily::pos() const
{
    return std::get<0>(m_memento->m_items);
}

QTextCharFormat FormatFamily::createCharFormat() const
{
	QTextCharFormat fmt;
	fmt.setFontFamily(std::get<1>(m_memento->m_items));
	return fmt;
}

const Memento* FormatFamily::getMemento() const
{
	return m_memento.get();
}

void FormatFamily::setMemento(MementoUP memento)
{
	auto casted = tools::unique_dyn_cast<FamilyMemento>(std::move(memento));

	if (casted)
	{
		m_memento = std::move(casted);
		return;
	}

	throwInvalidMemento(casted);
}
