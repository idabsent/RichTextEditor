#pragma once

#include "texteditoractions.hpp"

#include <QColor>
#include <QDataStream>

template<typename Item>
struct OneItemMemento : public Memento
{
	OneItemMemento(Item item)
		: m_item{std::move(item)}
	{	}

	QByteArray toRaw() const override
	{
		QByteArray raw;
		QDataStream rawStream{ &raw, QIODevice::WriteOnly };
		rawStream << m_item;
		return raw;
	}

	void initFromRaw(QByteArray&& raw) override
	{
		QDataStream rawStream{ &raw, QIODevice::ReadOnly };
		rawStream >> m_item;
	}

protected:
	Item m_item;
};

struct EmptyMemento : public Memento
{
	QByteArray toRaw() const override
	{
		return QByteArray{};
	}

	void initFromRaw(QByteArray&& raw) override
	{	}
};

struct BoldMemento : public OneItemMemento<bool>
{
	BoldMemento(bool isBold);

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

struct ItalicMemento : public OneItemMemento<bool>
{
	ItalicMemento(bool isItalic);

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

struct UnderlineMemento : public OneItemMemento<bool>
{
	UnderlineMemento(bool isUnderline);

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

struct AlignLeftMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignLeftMementoUP = std::unique_ptr<AlignLeftMemento, std::default_delete<Memento>>;

struct FormatAlignLeft : public TextEditorFormatAction
{
	using TextEditorFormatAction::TextEditorFormatAction;

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	AlignLeftMementoUP m_memento;
};

struct AlignCenterMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignCenterMementoUP = std::unique_ptr<AlignCenterMemento, std::default_delete<Memento>>;

struct FormatAlignCenter : public TextEditorFormatAction
{
	using TextEditorFormatAction::TextEditorFormatAction;

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	AlignCenterMementoUP m_memento;
};

struct AlignRightMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignRightMementoUP = std::unique_ptr<AlignRightMemento, std::default_delete<Memento>>;

struct FormatAlignRight : public TextEditorFormatAction
{
	using TextEditorFormatAction::TextEditorFormatAction;

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	AlignRightMementoUP m_memento;
};

struct AlignJustifyMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignJustifyMementoUP = std::unique_ptr<AlignJustifyMemento, std::default_delete<Memento>>;

struct FormatAlignJustify : public TextEditorFormatAction
{
	using TextEditorFormatAction::TextEditorFormatAction;

	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	AlignJustifyMementoUP m_memento;
};

struct ColorMemento : public OneItemMemento<QColor>
{
	ColorMemento(QColor color);

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

struct UnderlineColorMemento : public OneItemMemento<QColor>
{
	UnderlineColorMemento(QColor color);

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

struct SizeMemento : public OneItemMemento<int>
{
	SizeMemento(int size);

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