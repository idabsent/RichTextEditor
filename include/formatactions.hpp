#pragma once

#include "texteditoractions.hpp"

#include <QColor>
#include <QDataStream>

struct BoldMemento : public StreamItemsMemento<int, bool>
{
	BoldMemento(QTextCursor const& cursor, bool isBold);

	ActionType getActionType() const override;

	friend struct FormatBold;
};

using BoldMementoUP = std::unique_ptr<BoldMemento, std::default_delete<Memento>>;

struct FormatBold : public TextEditorFormatAction
{
	FormatBold(bool isBold, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;
	void setMemento(MementoUP memento) override;

private:
	BoldMementoUP m_memento;
};

struct ItalicMemento : public StreamItemsMemento<int, bool>
{
	ItalicMemento(QTextCursor const& cursor, bool isItalic);

	ActionType getActionType() const override;

	friend struct FormatItalic;
};

using ItalicMementoUP = std::unique_ptr<ItalicMemento, std::default_delete<Memento>>;

struct FormatItalic : public TextEditorFormatAction
{
	FormatItalic(bool isItalic, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	ItalicMementoUP m_memento;
};

struct UnderlineMemento : public StreamItemsMemento<int, bool>
{
	UnderlineMemento(QTextCursor const& cursor, bool isUnderline);

	ActionType getActionType() const override;

	friend struct FormatUnderline;
};

using UnderlineMementoUP = std::unique_ptr<UnderlineMemento, std::default_delete<Memento>>;

struct FormatUnderline : public TextEditorFormatAction
{
	FormatUnderline(bool isUnderline, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	UnderlineMementoUP m_memento;
};

struct ColorMemento : public StreamItemsMemento<int, QColor>
{
	ColorMemento(QTextCursor const& cursor, QColor color);

	ActionType getActionType() const;

	friend struct FormatColor;
};

using ColorMementoUP = std::unique_ptr<ColorMemento, std::default_delete<Memento>>;

struct FormatColor : public TextEditorFormatAction
{
	FormatColor(QColor color, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento);

private:
	ColorMementoUP m_memento;
};

struct UnderlineColorMemento : public StreamItemsMemento<int, QColor>
{
	UnderlineColorMemento(QTextCursor const& cursor, QColor color);

	ActionType getActionType() const;

	friend struct FormatUnderlineColor;
};

using UnderlineColorMementoUP = std::unique_ptr<UnderlineColorMemento, std::default_delete<Memento>>;

struct FormatUnderlineColor : public TextEditorFormatAction
{
	FormatUnderlineColor(QColor color, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento);

private:
	UnderlineColorMementoUP m_memento;
};

struct SizeMemento : public StreamItemsMemento<int, int>
{
	SizeMemento(QTextCursor const& cursor, int size);

	ActionType getActionType() const override;

	friend class FormatSize;
};

using SizeMementoUP = std::unique_ptr<SizeMemento, std::default_delete<Memento>>;

struct FormatSize : public TextEditorFormatAction
{
	FormatSize(int size, QTextEdit* textEditor);

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	SizeMementoUP m_memento;
};