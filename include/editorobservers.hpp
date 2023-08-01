#pragma once

#include <QTextEdit>

#include "actions.hpp"

struct AdditionalEmiterTextEditor : public QTextEdit
{
	using QTextEdit::QTextEdit;

signals:
	void symAdded(QChar const& txt);
	void symRemoved(QChar const& txt);

protected:
	void keyReleaseEvent(QKeyEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	Q_OBJECT

	QChar nextChar();
	QChar previousChar();
};

struct TextChangeObserver : public QObject
{
	TextChangeObserver(AdditionalEmiterTextEditor* textEditor, size_t buffSize);

public slots:
	void onSymAdded(QChar const& sym);
    void onSymRemoved(QChar const& sym);

private:
	Q_OBJECT

	AdditionalEmiterTextEditor* m_editor;
};

struct DBusActionsObserver : public QObject
{
	DBusActionsObserver(QTextEdit* textEditor, QObject* parent = nullptr);

private slots:
	void onDataReceived(ActionType action, QByteArray const& raw);

private:
	Q_OBJECT

	QTextEdit* m_editor;
};
