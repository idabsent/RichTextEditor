#pragma once

#include "texteditoractions.hpp"

#include <QColor>
#include <QDataStream>

struct BoldMemento : public StreamItemsMemento<int, int, bool>
{
    BoldMemento() = default;
	BoldMemento(QTextCursor const& cursor, bool isBold);

	ActionType getActionType() const override;

	friend struct FormatBold;
};

using BoldMementoUP = std::unique_ptr<BoldMemento, std::default_delete<Memento>>;

struct FormatBold : public FormatAction
{
	FormatBold(bool isBold, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatBold(QTextEdit* textEditor);

	BoldMementoUP m_memento;
};

struct ItalicMemento : public StreamItemsMemento<int, int, bool>
{
    ItalicMemento() = default;
	ItalicMemento(QTextCursor const& cursor, bool isItalic);

	ActionType getActionType() const override;

	friend struct FormatItalic;
};

using ItalicMementoUP = std::unique_ptr<ItalicMemento, std::default_delete<Memento>>;

struct FormatItalic : public FormatAction
{
	FormatItalic(bool isItalic, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatItalic(QTextEdit* textEditor);

	ItalicMementoUP m_memento;
};

struct UnderlineMemento : public StreamItemsMemento<int, int, bool>
{
    UnderlineMemento() = default;
	UnderlineMemento(QTextCursor const& cursor, bool isUnderline);

	ActionType getActionType() const override;

    friend struct FormatUnderline;
};

using UnderlineMementoUP = std::unique_ptr<UnderlineMemento, std::default_delete<Memento>>;

struct FormatUnderline : public FormatAction
{
	FormatUnderline(bool isUnderline, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatUnderline(QTextEdit* textEditor);

	UnderlineMementoUP m_memento;
};

struct ColorMemento : public StreamItemsMemento<int, int, QColor>
{
    ColorMemento() = default;
	ColorMemento(QTextCursor const& cursor, QColor color);

	ActionType getActionType() const;

	friend struct FormatColor;
};

using ColorMementoUP = std::unique_ptr<ColorMemento, std::default_delete<Memento>>;

struct FormatColor : public FormatAction
{
	FormatColor(QColor color, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatColor(QTextEdit* textEditor);

	ColorMementoUP m_memento;
};

struct UnderlineColorMemento : public StreamItemsMemento<int, int, QColor>
{
    UnderlineColorMemento() = default;
	UnderlineColorMemento(QTextCursor const& cursor, QColor color);

	ActionType getActionType() const;

	friend struct FormatUnderlineColor;
};

using UnderlineColorMementoUP = std::unique_ptr<UnderlineColorMemento, std::default_delete<Memento>>;

struct FormatUnderlineColor : public FormatAction
{
	FormatUnderlineColor(QColor color, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatUnderlineColor(QTextEdit* textEditor);

	UnderlineColorMementoUP m_memento;
};

struct SizeMemento : public StreamItemsMemento<int, int, int>
{
    SizeMemento() = default;
	SizeMemento(QTextCursor const& cursor, int size);

	ActionType getActionType() const override;

	friend class FormatSize;
};

using SizeMementoUP = std::unique_ptr<SizeMemento, std::default_delete<Memento>>;

struct FormatSize : public FormatAction
{
	FormatSize(int size, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatSize(QTextEdit* textEditor);

	SizeMementoUP m_memento;
};

struct FamilyMemento : public StreamItemsMemento<int, int, QString>
{
	FamilyMemento() = default;
	FamilyMemento(QTextCursor const& cursor, QString const& family);

	ActionType getActionType() const override;

	friend struct FormatFamily;
};

using FamilyMementoUP = std::unique_ptr<FamilyMemento, std::default_delete<Memento>>;

struct FormatFamily : public FormatAction
{
	FormatFamily(QString const& family, QTextEdit* textEditor);

    int posBegin() const override;
    int posEnd() const override;
	QTextCharFormat createCharFormat() const override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	friend struct GlobalMementoBuilder;

	FormatFamily(QTextEdit* textEditor);

	FamilyMementoUP m_memento;
};
