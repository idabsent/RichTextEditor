#include "texteditoractions.hpp"
#include "formatactions.hpp"
#include "tools.hpp"

#include <exception>
#include <cerrno>
#include <cstring>

#include <QDataStream>
#include <QTextStream>
#include <QTextList>
#include <QFile>
#include <QApplication>

std::unique_ptr<GlobalMementoBuilder> GlobalMementoBuilder::_instance;

GlobalMementoBuilder::GlobalMementoBuilder(EditorTabWidget* docsEditor)
    : m_docsEditor{docsEditor}
{	}

bool GlobalMementoBuilder::actionIsSupported(ActionType action) const
{
	switch (action)
	{
	case ActionType::FormatBold:
	case ActionType::FormatItalic:
	case ActionType::FormatUnderline:
	case ActionType::FormatAlignLeft:
	case ActionType::FormatAlignRight:
	case ActionType::FormatAlignCenter:
	case ActionType::FormatAlignJustify:
	case ActionType::FormatIndent:
	case ActionType::FormatUnindent:
	case ActionType::FormatColor:
    case ActionType::FormatUnderlineColor:
    case ActionType::FileOpen:
    case ActionType::FileSave:
    case ActionType::FileSaveAs:
    case ActionType::DocNew:
    case ActionType::TextChange:
    case ActionType::EditCopy:
    case ActionType::EditCut:
    case ActionType::EditPaste:
    case ActionType::EditRedo:
    case ActionType::EditUndo:
		return true;
	default:
		return false;
	}
}

//TODO enable support other action mementos
ActionUP GlobalMementoBuilder::deserializeAction(QByteArray&& data, ActionType type)
{
	switch (auto editor = m_docsEditor->getEditor(); type)
	{
    case ActionType::FormatBold:
        return build<FormatBold, BoldMemento>(std::move(data), editor);
    case ActionType::FormatItalic:
        return build<FormatItalic, ItalicMemento>(std::move(data), editor);
	case ActionType::FormatUnderline:
        return build<FormatUnderline, UnderlineMemento>(std::move(data), editor);
	case ActionType::FormatAlignLeft:
        return build<FormatAlignLeft, AlignLeftMemento>(std::move(data), editor);
	case ActionType::FormatAlignRight:
        return build<FormatAlignRight, AlignRightMemento>(std::move(data), editor);
	case ActionType::FormatAlignCenter:
        return build<FormatAlignCenter, AlignCenterMemento>(std::move(data), editor);
	case ActionType::FormatAlignJustify:
        return build<FormatAlignJustify, AlignJustifyMemento>(std::move(data), editor);
    case ActionType::FormatIndent:
	case ActionType::FormatUnindent:
        return build<FormatIndent, IndentMemento>(std::move(data), editor);
	case ActionType::FormatColor:
        return build<FormatColor, ColorMemento>(std::move(data), editor);
	case ActionType::FormatUnderlineColor:
        return build<FormatUnderlineColor, UnderlineColorMemento>(std::move(data), editor);
    case ActionType::FileOpen:
        return build<FileOpenAction, FileOpenMemento>(std::move(data), m_docsEditor);
    case ActionType::FileSave:
        return build<FileSaveAction, FileSaveMemento>(std::move(data), m_docsEditor);
    case ActionType::FileSaveAs:
        return build<FileSaveAsAction, FileSaveAsMemento>(std::move(data), m_docsEditor);
    case ActionType::DocNew:
        return build<DocNewAction, DocNewMemento>(std::move(data), m_docsEditor);
    case ActionType::EditCopy:
        return build<EditCopyAction, EditCopyMemento>(std::move(data), editor);
    case ActionType::EditPaste:
        return build<EditPasteAction, EditPasteMemento>(std::move(data), editor);
    case ActionType::EditCut:
        return build<EditCutAction, EditCutMemento>(std::move(data), editor);
    case ActionType::EditUndo:
        return build<EditUndoAction, EditUndoMemento>(std::move(data), editor);
    case ActionType::EditRedo:
        return build<EditRedoAction, EditRedoMemento>(std::move(data), editor);
    case ActionType::FontSize:
        return build<FormatSize, SizeMemento>(std::move(data), editor);
    case ActionType::FontFamily:
        return build<FormatFamily, FamilyMemento>(std::move(data), editor);
    case ActionType::TextChange:
        return build<TextChangeAction_1, TextChangeAction_1::MementoInner>(std::move(data), editor);
    default:
        auto errorMsg = QString{"%1: Attemption build memento from unsupported ActionType{%2}"}
                .arg(FUNC_SIGN)
                .arg(static_cast<int>(type));
        throw std::logic_error{errorMsg.toStdString()};
	}
}

void GlobalMementoBuilder::createInstance(EditorTabWidget* docsEditor)
{
    auto builder = new GlobalMementoBuilder{ docsEditor };
    _instance.reset(builder);
}

GlobalMementoBuilder* GlobalMementoBuilder::instance()
{
	if (!_instance)
	{
		throw std::logic_error{"Attemption get uninitializaed instance..."};
	}

	return _instance.get();
}

FormatAction::FormatAction(QTextEdit* textEditor)
	: m_editor{textEditor}
{	}

void FormatAction::execute()
{
	mergeFormat(createCharFormat());
}

void FormatAction::mergeFormat(QTextCharFormat const& fmt)
{
    auto m_posBegin = posBegin();
    auto m_posEnd = posEnd();

    auto cursor = m_editor->textCursor();

    if (m_posBegin == m_posEnd)
    {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    else
    {
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, m_posBegin);
        cursor.selectionStart();
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_posEnd);
        cursor.selectionEnd();
    }

	cursor.mergeCharFormat(fmt);
	m_editor->mergeCurrentCharFormat(fmt);
}

IndentMemento::IndentMemento(QTextCursor const& cursor, int indent)
    : IndentMemento{cursor.position(), indent}
{	}

IndentMemento::IndentMemento(int pos, int indent)
    : StreamItemsMemento{pos, indent}
{	}

ActionType IndentMemento::getActionType() const
{
    return std::get<1>(m_items) >= 0 ? ActionType::FormatIndent : ActionType::FormatUnindent;
}

FormatIndent::FormatIndent(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{nullptr}
{   }

FormatIndent::FormatIndent(int indent, QTextEdit* textEditor)
	: m_editor{textEditor}
    , m_memento{std::make_unique<IndentMemento>(textEditor->textCursor(), indent)}
{	}

//TODO
void FormatIndent::execute()
{
    auto indent = std::get<1>(m_memento->m_items);

    QTextCursor cursor = m_editor->textCursor();
    cursor.beginEditBlock();
    if (cursor.currentList())
    {
        QTextListFormat listFmt = cursor.currentList()->format();
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() && listFmt.indent() + indent == above.currentList()->format().indent())
        {
            above.currentList()->add(cursor.block());
        }
        else
        {
            listFmt.setIndent(listFmt.indent() + indent);
            cursor.createList(listFmt);
        }
    }
    else
    {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + indent);
        cursor.setBlockFormat(blockFmt);
    }
    cursor.endEditBlock();
}

const Memento* FormatIndent::getMemento() const
{
	return m_memento.get();
}

void FormatIndent::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<IndentMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(memento);
}

CheckedMemento::CheckedMemento(QTextCursor const& cursor, bool isChecked)
    : CheckedMemento{cursor.position(), isChecked}
{	}

CheckedMemento::CheckedMemento(int pos, bool isChecked)
    : StreamItemsMemento{pos, isChecked}
{	}

ActionType CheckedMemento::getActionType() const
{
	return ActionType::FormatChecked;
}

FormatChecked::FormatChecked(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{nullptr}
{   }

FormatChecked::FormatChecked(bool checked, QTextEdit* textEditor)
	: m_editor{ textEditor }
    , m_memento{ std::make_unique< CheckedMemento>(textEditor->textCursor(), checked) }
{	}

//TODO implement action
void FormatChecked::execute()
{
	
}

const Memento* FormatChecked::getMemento() const
{
	return m_memento.get();
}

void FormatChecked::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<CheckedMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}


ActionType AlignLeftMemento::getActionType() const
{
	return ActionType::FormatAlignLeft;
}

FormatAlignLeft::FormatAlignLeft(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{std::make_unique<AlignLeftMemento>()}
{   }

//TODO
void FormatAlignLeft::execute()
{
    m_editor->setAlignment(Qt::AlignLeft);
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

FormatAlignCenter::FormatAlignCenter(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{std::make_unique<AlignCenterMemento>()}
{   }

//TODO
void FormatAlignCenter::execute() 
{
    m_editor->setAlignment(Qt::AlignCenter);
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

FormatAlignRight::FormatAlignRight(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{std::make_unique<AlignRightMemento>()}
{   }

//TODO
void FormatAlignRight::execute()
{
    m_editor->setAlignment(Qt::AlignRight);
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

FormatAlignJustify::FormatAlignJustify(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{std::make_unique<AlignJustifyMemento>()}
{   }

//TODO
void FormatAlignJustify::execute()
{
    m_editor->setAlignment(Qt::AlignJustify);
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

TextChangeMemento::TextChangeMemento(QTextCursor const& cursor, TextChangeType type, QChar const& chr)
    : TextChangeMemento{cursor.position(), type, chr}
{   }

TextChangeMemento::TextChangeMemento(int pos, TextChangeType type, QChar const& chr)
    : StreamItemsMemento{pos, static_cast<int>(type), chr}
{   }

ActionType TextChangeMemento::getActionType() const
{
    return ActionType::TextChange;
}

TextChangeAction::TextChangeAction(QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{nullptr}
{   }

TextChangeAction::TextChangeAction(TextChangeType type, QChar const& chr, QTextEdit* textEditor)
    : m_editor{textEditor}
    , m_memento{std::make_unique<TextChangeMemento>(textEditor->textCursor(), type, chr)}
{   }

void TextChangeAction::execute()
{
    auto m_pos = std::get<0>(m_memento->m_items);
    auto action = static_cast<TextChangeType>(std::get<1>(m_memento->m_items));
    auto m_chr = std::get<2>(m_memento->m_items);

    auto cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, m_pos);

    switch (action)
    {
    case TextChangeType::Added:
        cursor.insertText(QString{ m_chr });
        break;
    case TextChangeType::Removed:
        cursor.deleteChar();
        break;
    }
}

const Memento* TextChangeAction::getMemento() const
{
    return m_memento.get();
}

void TextChangeAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<TextChangeMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

FileOpenMemento::FileOpenMemento(QString const& path)
    : StreamItemsMemento{path}
{   }

ActionType FileOpenMemento::getActionType() const
{
    return ActionType::FileOpen;
}

FileOpenAction::FileOpenAction(EditorTabWidget* docsEditor)
    : m_memento{nullptr}
    , m_docsEditor{docsEditor}
{   }

FileOpenAction::FileOpenAction(QString const& path, EditorTabWidget* docsEditor)
    : m_memento{ std::make_unique<FileOpenMemento>(path) }
    , m_docsEditor{docsEditor}
{   }

const Memento* FileOpenAction::getMemento() const
{
    return m_memento.get();
}

//TODO implement
void FileOpenAction::execute()
{
    auto path = std::get<0>(m_memento->m_items);

    QFile file{path};
    if (!file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error{
            QString{"Can't open file{%1}. Error{%2}"}
                .arg(path)
                .arg(std::strerror(errno))
                .toStdString()
        };
    }

    QTextStream file_stream{&file};
    auto content = file_stream.readAll();
    auto doc = new QTextDocument;
    doc->setHtml(content);

    m_docsEditor->addDocument(path, doc);
}

void FileOpenAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<FileOpenMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

ActionType DocNewMemento::getActionType() const
{
    return ActionType::DocNew;
}

DocNewAction::DocNewAction(EditorTabWidget* docsEditor)
    : m_memento{std::make_unique<DocNewMemento>()}
    , m_docsEditor{docsEditor}
{   }

const Memento* DocNewAction::getMemento() const
{
    return m_memento.get();
}

void DocNewAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<DocNewMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

void DocNewAction::execute()
{
    m_docsEditor->addDocument(QApplication::tr("Untitled"), new QTextDocument);
}

FileSaveAsMemento::FileSaveAsMemento(QString const& path)
    : StreamItemsMemento{path}
{   }

ActionType FileSaveAsMemento::getActionType() const
{
    return ActionType::FileSaveAs;
}

FileSaveAsAction::FileSaveAsAction(EditorTabWidget* docsEditor)
    : m_memento{nullptr}
    , m_docsEditor{docsEditor}
{   }

FileSaveAsAction::FileSaveAsAction(QString const& path, EditorTabWidget* docsEditor)
    : m_memento{std::make_unique<FileSaveAsMemento>(path)}
    , m_docsEditor{docsEditor}
{   }

//TODO implementation
void FileSaveAsAction::execute()
{
    auto path = std::get<0>(m_memento->m_items);
    QFile file{path};

    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error{
            QString{"Cann't open file. Error{%1}"}
                .arg(std::strerror(errno))
                .toStdString()
        };
    }

    QTextStream file_stream{&file};
    auto doc = m_docsEditor->getCurrentDocument();
    auto content = doc->toHtml();
    file_stream << content;
    file_stream.flush();
    m_docsEditor->changeCurrentTitle(path);
}

const Memento* FileSaveAsAction::getMemento() const
{
    return m_memento.get();
}

void FileSaveAsAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<FileSaveAsMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

FileSaveMemento::FileSaveMemento(QString const& title)
    : StreamItemsMemento{title}
{   }

ActionType FileSaveMemento::getActionType() const
{
    return ActionType::FileSave;
}

FileSaveAction::FileSaveAction(EditorTabWidget* docsEditor)
    : m_memento{nullptr}
    , m_docsEditor{docsEditor}
{   }

FileSaveAction::FileSaveAction(QString const& docTitle, EditorTabWidget* docsEditor)
    : m_memento{std::make_unique<FileSaveMemento>(docTitle)}
    , m_docsEditor{docsEditor}
{   }

//TODO implementation
void FileSaveAction::execute()
{
    auto path = std::get<0>(m_memento->m_items);
    auto document = m_docsEditor->getCurrentDocument();
    auto content = document->toHtml();

    QFile file{path};
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error{
            QString{"Can't open file. Error{%1}"}
                .arg(std::strerror(errno))
                .toStdString()
        };
    }

    QTextStream file_stream{&file};
    file_stream << content;
    file_stream.flush();

    m_docsEditor->changeCurrentTitle(path);
}

const Memento* FileSaveAction::getMemento() const
{
    return m_memento.get();
}

void FileSaveAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<FileSaveMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

EditCopyMemento::EditCopyMemento(QTextCursor const& cursor)
    : StreamItemsMemento{cursor.selectionStart(), cursor.selectionEnd()}
{   }

ActionType EditCopyMemento::getActionType() const
{
    return ActionType::EditCopy;
}

EditCopyAction::EditCopyAction(QTextEdit* textEditor)
    : m_textEditor{textEditor}
    , m_memento{std::make_unique<EditCopyMemento>(textEditor->textCursor())}
{   }

void EditCopyAction::execute()
{
    TODO("implement");
}

const Memento* EditCopyAction::getMemento() const
{
    return m_memento.get();
}

void EditCopyAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<EditCopyMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

EditCutMemento::EditCutMemento(QTextCursor const& cursor)
    : StreamItemsMemento{cursor.selectionStart(), cursor.selectionEnd()}
{   }

ActionType EditCutMemento::getActionType() const
{
    return ActionType::EditCut;
}

EditCutAction::EditCutAction(QTextEdit* textEditor)
    : m_textEditor{textEditor}
    , m_memento{std::make_unique<EditCutMemento>(textEditor->textCursor())}
{   }

void EditCutAction::execute()
{
    TODO("implement");
}

const Memento* EditCutAction::getMemento() const
{
    return m_memento.get();
}

void EditCutAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast_new<EditCutMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

EditPasteMemento::EditPasteMemento(QTextCursor const& cursor)
    : StreamItemsMemento{cursor.selectionStart(), cursor.selectionEnd()}
{   }

ActionType EditPasteMemento::getActionType() const
{
    return ActionType::EditPaste;
}

EditPasteAction::EditPasteAction(QTextEdit* textEditor)
    : m_textEditor{textEditor}
    , m_memento{std::make_unique<EditPasteMemento>(textEditor->textCursor())}
{   }

void EditPasteAction::execute()
{
    TODO("implement");
}

const Memento* EditPasteAction::getMemento() const
{
    return m_memento.get();
}

void EditPasteAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<EditPasteMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

ActionType EditRedoMemento::getActionType() const
{
    return ActionType::EditRedo;
}

EditRedoAction::EditRedoAction(QTextEdit* textEditor)
    : m_textEditor{textEditor}
    , m_memento{std::make_unique<EditRedoMemento>()}
{   }

void EditRedoAction::execute()
{
    TODO("implement");
}

const Memento* EditRedoAction::getMemento() const
{
    return m_memento.get();
}

void EditRedoAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<EditRedoMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}

ActionType EditUndoMemento::getActionType() const
{
    return ActionType::EditUndo;
}

EditUndoAction::EditUndoAction(QTextEdit* textEditor)
    : m_textEditor{textEditor}
    , m_memento{std::make_unique<EditUndoMemento>()}
{   }

void EditUndoAction::execute()
{
    TODO("implement");
}

const Memento* EditUndoAction::getMemento() const
{
    return m_memento.get();
}

void EditUndoAction::setMemento(MementoUP memento)
{
    auto casted = tools::unique_dyn_cast<EditUndoMemento>(std::move(memento));

    if (casted)
    {
        m_memento = std::move(casted);
        return;
    }

    throwInvalidMemento(casted);
}
