#pragma once

#include "actions.hpp"
#include "tools.hpp"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextEdit>

#include <type_traits>
#include <tuple>

template<typename Item>
struct OneItemMemento : public Memento
{
	OneItemMemento(Item item)
		: m_item{ std::move(item) }
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

template <typename... Items>
struct StreamItemsMemento : public Memento
{
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

struct TextEditorFormatMementoBuilder : public MementoBuilder
{
	bool supportAction(ActionType action) const override;
	ActionUP buildAction(MementoUP memento) override;
	MementoUP buildMemento(QByteArray&& data, ActionType action) override;

	static void createInstance(QTextEdit* editor);
	static TextEditorFormatMementoBuilder* instance();

private:
	static std::unique_ptr<TextEditorFormatMementoBuilder> _instance;
	TextEditorFormatMementoBuilder(QTextEdit* editor);

	QTextEdit* m_editor;
};

struct TextEditorFormatAction : public Action
{
	TextEditorFormatAction(QTextEdit* textEditor);

	void execute() override;

	virtual QTextCharFormat createCharFormat() const = 0;

private:
	friend class TextEditorFormatMementoBuilder;

	void mergeFormat(QTextCharFormat const& fmt);

	QTextEdit* m_editor;
};

struct TextEditorFormatIndentMemento : public Memento
{
	TextEditorFormatIndentMemento(QTextCursor const& cursor, int indent);
	TextEditorFormatIndentMemento(int pos, int indent);

	ActionType getActionType() const override;
	QByteArray toRaw() const override;
	void initFromRaw(QByteArray&& raw) override;

private:
	friend class TextEditorFormatIndentAction;

	int m_pos;
	int m_indent;
	ActionType m_action;
};

struct TextEditorFormatIndentAction : public Action
{
	TextEditorFormatIndentAction(int indent, QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	QTextEdit* m_editor;
	MementoUP m_memento;
};

struct TextEditorFormatCheckedMemento : public Memento
{
	TextEditorFormatCheckedMemento(QTextCursor const& cursor, bool isChecked);
	TextEditorFormatCheckedMemento(int pos, bool isChecked);

	ActionType getActionType() const override;
	QByteArray toRaw() const override;
	void initFromRaw(QByteArray&& raw) override;

private:
	friend class TextEditorFormatCheckedAction;

	int m_pos;
	bool m_checked;
};

struct TextEditorFormatCheckedAction : public Action
{
	TextEditorFormatCheckedAction(bool checked, QTextEdit* textEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	QTextEdit* m_editor;
	MementoUP m_memento;
};


struct AlignLeftMemento : public EmptyMemento
{
	ActionType getActionType() const override;
};

using AlignLeftMementoUP = std::unique_ptr<AlignLeftMemento, std::default_delete<Memento>>;

struct FormatAlignLeft : public Action
{
	void execute() override;
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

struct FormatAlignCenter : public Action
{
	void execute() override;
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

struct FormatAlignRight : public Action
{
	void execute() override;
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

struct FormatAlignJustify : public Action
{
	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	AlignJustifyMementoUP m_memento;
};