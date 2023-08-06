#pragma once

#include "actions.hpp"
#include "tools.hpp"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextEdit>

#include <type_traits>
#include <tuple>

template <typename... Items>
struct StreamItemsMemento : public Memento
{
    StreamItemsMemento()
        : m_items{ Items{}... }
    {   }

	StreamItemsMemento(Items... items)
		: m_items{ std::move(items)... }
	{	}

	QByteArray toRaw() const override
	{
		QByteArray raw;
		QDataStream rawStream{ &raw, QIODevice::WriteOnly };

		writeToStream(rawStream, m_items);

		return raw;
	}

	void initFromRaw(QByteArray&& raw) override
	{
		QDataStream rawStream{ &raw, QIODevice::ReadOnly };
		readFromStream(rawStream, m_items);
	}

private:
	template<size_t Ind = 0, typename... Tpl>
	typename std::enable_if<Ind == sizeof...(Tpl), void>::type writeToStream(QDataStream& str, std::tuple<Tpl...> const& tpl) const
	{	}

	template<size_t Ind = 0, typename... Tpl>
	typename std::enable_if < Ind < sizeof...(Tpl), void>::type writeToStream(QDataStream& str, std::tuple<Tpl...> const& tpl) const
	{
		str << std::get<Ind>(tpl);
		writeToStream<Ind + 1, Tpl...>(str, tpl);
	}

	template<size_t Ind = 0, typename... Tpl>
	typename std::enable_if<Ind == sizeof...(Tpl), void>::type readFromStream(QDataStream& str, std::tuple<Tpl...>& tpl)
	{	}

	template<size_t Ind = 0, typename... Tpl>
	typename std::enable_if < Ind < sizeof...(Tpl), void>::type readFromStream(QDataStream& str, std::tuple<Tpl...>& tpl)
	{
		str >> std::get<Ind>(tpl);
		readFromStream<Ind + 1, Tpl...>(str, tpl);
	}

protected:
	std::tuple<Items...> m_items;
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

struct GlobalMementoBuilder : public MementoBuilder
{
	bool supportAction(ActionType action) const override;
	ActionUP buildAction(MementoUP memento) override;
	MementoUP buildMemento(QByteArray&& data, ActionType action) override;

	static void createInstance(QTextEdit* editor);
    static GlobalMementoBuilder* instance();

private:
    static std::unique_ptr<GlobalMementoBuilder> _instance;
    GlobalMementoBuilder(QTextEdit* editor);

	QTextEdit* m_editor;
};

struct FormatAction : public Action
{
    FormatAction(QTextEdit* textEditor);

	void execute() override;

    virtual int posBegin() const = 0;
    virtual int posEnd() const = 0;
	virtual QTextCharFormat createCharFormat() const = 0;

private:
    friend struct GlobalMementoBuilder;

	void mergeFormat(QTextCharFormat const& fmt);

	QTextEdit* m_editor;
};

struct IndentMemento : public StreamItemsMemento<int, int>
{
    IndentMemento() = default;
    IndentMemento(QTextCursor const& cursor, int indent);
    IndentMemento(int pos, int indent);

    ActionType getActionType() const override;

private:
    friend struct FormatIndent;
};

using IndentMementoUP = std::unique_ptr<IndentMemento, std::default_delete<Memento>>;

struct FormatIndent : public Action
{
    FormatIndent(int indent, QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatIndent(QTextEdit* textEditor);

	QTextEdit* m_editor;
    IndentMementoUP m_memento;
};

struct CheckedMemento : public StreamItemsMemento<int, bool>
{
    CheckedMemento() = default;
    CheckedMemento(QTextCursor const& cursor, bool isChecked);
    CheckedMemento(int pos, bool isChecked);

    ActionType getActionType() const override;
};

using CheckedMementoUP = std::unique_ptr<CheckedMemento, std::default_delete<Memento>>;

struct FormatChecked : public Action
{
    FormatChecked(bool checked, QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    FormatChecked(QTextEdit* textEditor);

	QTextEdit* m_editor;
    CheckedMementoUP m_memento;
};


struct AlignLeftMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignLeftMementoUP = std::unique_ptr<AlignLeftMemento, std::default_delete<Memento>>;

struct FormatAlignLeft : public Action
{
    FormatAlignLeft(QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    QTextEdit* m_editor;
	AlignLeftMementoUP m_memento;
};

struct AlignCenterMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignCenterMementoUP = std::unique_ptr<AlignCenterMemento, std::default_delete<Memento>>;

struct FormatAlignCenter : public Action
{
    FormatAlignCenter(QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    QTextEdit* m_editor;
	AlignCenterMementoUP m_memento;
};

struct AlignRightMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignRightMementoUP = std::unique_ptr<AlignRightMemento, std::default_delete<Memento>>;

struct FormatAlignRight : public Action
{
    FormatAlignRight(QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    QTextEdit* m_editor;
	AlignRightMementoUP m_memento;
};

struct AlignJustifyMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignJustifyMementoUP = std::unique_ptr<AlignJustifyMemento, std::default_delete<Memento>>;

struct FormatAlignJustify : public Action
{
    FormatAlignJustify(QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
    friend struct GlobalMementoBuilder;

    QTextEdit* m_editor;
	AlignJustifyMementoUP m_memento;
};

enum class TextChangeType : int
{
	Removed,
	Added,
};

struct TextChangeMemento : public StreamItemsMemento<int, int, QChar>
{
	TextChangeMemento() = default;
    TextChangeMemento(QTextCursor const& cursor, TextChangeType type, QChar const& chr);
    TextChangeMemento(int pos, TextChangeType type, QChar const& chr);

	ActionType getActionType() const override;

	friend struct TextChangeAction;
};

using TextChangedMementoUP = std::unique_ptr<TextChangeMemento, std::default_delete<Memento>>;

struct TextChangeAction : public Action
{
    TextChangeAction(QTextEdit* textEditor);
    TextChangeAction(TextChangeType type, QChar const& chr, QTextEdit* textEditor);

	void execute() override;
    const Memento* getMemento() const;

protected:
    void setMemento(MementoUP memento);

private:
	friend struct GlobalMementoBuilder;

	QTextEdit* m_editor;
	TextChangedMementoUP m_memento;
};
