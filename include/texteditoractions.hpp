#pragma once

#include "actions.hpp"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextEdit>

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