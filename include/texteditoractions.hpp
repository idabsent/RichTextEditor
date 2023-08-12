#pragma once

#include "actions.hpp"
#include "tools.hpp"
#include "editortabwidget.hpp"

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

    static void createInstance(EditorTabWidget* docsEditor);
    static GlobalMementoBuilder* instance();

private:
    static std::unique_ptr<GlobalMementoBuilder> _instance;
    GlobalMementoBuilder(EditorTabWidget* docsEditor);

    EditorTabWidget* m_docsEditor;
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

struct FileOpenMemento : public StreamItemsMemento<QString>
{
    FileOpenMemento() = default;
    FileOpenMemento(QString const& path);

	ActionType getActionType() const override;

	friend struct FileOpenAction;
};

using FileOpenMementoUP = std::unique_ptr<FileOpenMemento, std::default_delete<Memento>>;

struct FileOpenAction : public Action
{
	FileOpenAction(EditorTabWidget* docsEditor);
    FileOpenAction(QString const& path, EditorTabWidget* docsEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	EditorTabWidget* m_docsEditor;
	FileOpenMementoUP m_memento;

	friend struct GlobalMementoBuilder;
};

struct DocNewMemento : public EmptyMemento
{
    DocNewMemento() = default;
	ActionType getActionType() const override;

	friend struct DocNewAction;
};

using DocNewMementoUP = std::unique_ptr<DocNewMemento, std::default_delete<Memento>>;

struct DocNewAction : public Action
{
	DocNewAction(EditorTabWidget* docsEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	friend struct GlobalMementoBuilder;

	EditorTabWidget* m_docsEditor;
	DocNewMementoUP m_memento;
};

struct FileSaveAsMemento : public StreamItemsMemento<QString>
{
    FileSaveAsMemento() = default;
    FileSaveAsMemento(QString const& path);

	ActionType getActionType() const override;

	friend struct FileSaveAsAction;
};

using FileSaveAsMementoUP = std::unique_ptr<FileSaveAsMemento, std::default_delete<Memento>>;

struct FileSaveAsAction : public Action
{
	FileSaveAsAction(EditorTabWidget* editor);
    FileSaveAsAction(QString const& path, EditorTabWidget* docsEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	EditorTabWidget* m_docsEditor;
	FileSaveAsMementoUP m_memento;

	friend struct GlobalMementoBuilder;
};

struct FileSaveMemento : public StreamItemsMemento<QString>
{
    FileSaveMemento() = default;
    FileSaveMemento(QString const& title);

	ActionType getActionType() const override;

	friend struct FileSaveAction;
};

using FileSaveMementoUP = std::unique_ptr<FileSaveMemento, std::default_delete<Memento>>;

struct FileSaveAction : public Action
{
	FileSaveAction(EditorTabWidget* docsEditor);
    FileSaveAction(QString const& docTitle, EditorTabWidget* docsEditor);

	void execute() override;
	const Memento* getMemento() const override;

protected:
	void setMemento(MementoUP memento) override;

private:
	FileSaveMementoUP m_memento;
	EditorTabWidget* m_docsEditor;

	friend struct GlobalMementoBuilder;
};

struct EditCopyMemento : public StreamItemsMemento<int, int>
{
    EditCopyMemento() = default;
    EditCopyMemento(QTextCursor const& cursor);
    ActionType getActionType() const override;

    friend struct EditCopyAction;
};

using EditCopyMementoUP = std::unique_ptr<EditCopyMemento, std::default_delete<Memento>>;

struct EditCopyAction : public Action
{
    EditCopyAction(QTextEdit* textEditor);

    void execute() override;
    const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    QTextEdit* m_textEditor;
    EditCopyMementoUP m_memento;

    friend struct GlobalMementoBuilder;
};

struct EditCutMemento : public StreamItemsMemento<int, int>
{
    EditCutMemento() = default;
    EditCutMemento(QTextCursor const& cursor);

    ActionType getActionType() const override;
};

using EditCutMementoUP = std::unique_ptr<EditCutMemento, std::default_delete<Memento>>;

struct EditCutAction : public Action
{
    EditCutAction(QTextEdit* textEditor);

    void execute() override;
    const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    QTextEdit* m_textEditor;
    EditCutMementoUP m_memento;

    friend struct GlobalMementoBuilder;
};

struct EditPasteMemento : public StreamItemsMemento<int, int>
{
    EditPasteMemento() = default;
    EditPasteMemento(QTextCursor const& cursor);

    ActionType getActionType() const override;

    friend struct EditPasteAction;
};

using EditPasteMementoUP = std::unique_ptr<EditPasteMemento, std::default_delete<Memento>>;

struct EditPasteAction : public Action
{
    EditPasteAction(QTextEdit* textEditor);

    void execute() override;
    const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    QTextEdit* m_textEditor;
    EditPasteMementoUP m_memento;

    friend struct GlobalMementoBuilder;
};

struct EditRedoMemento : public EmptyMemento
{
    EditRedoMemento() = default;

    ActionType getActionType() const override;
};

using EditRedoMementoUP = std::unique_ptr<EditRedoMemento, std::default_delete<Memento>>;

struct EditRedoAction : public Action
{
    EditRedoAction(QTextEdit* textEditor);

    void execute() override;
    const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    QTextEdit* m_textEditor;
    EditRedoMementoUP m_memento;

    friend struct GlobalMementoBuilder;
};

struct EditUndoMemento : public EmptyMemento
{
    EditUndoMemento() = default;

    ActionType getActionType() const override;
};

using EditUndoMementoUP = std::unique_ptr<EditUndoMemento, std::default_delete<Memento>>;

struct EditUndoAction : public Action
{
    EditUndoAction(QTextEdit* textEditor);

    void execute() override;
    const Memento* getMemento() const override;

protected:
    void setMemento(MementoUP memento) override;

private:
    QTextEdit* m_textEditor;
    EditUndoMementoUP m_memento;

    friend struct GlobalMementoBuilder;
};
