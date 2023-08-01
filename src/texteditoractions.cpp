#include "texteditoractions.hpp"
#include "formatactions.hpp"
#include "tools.hpp"

#include <exception>
#include <QDataStream>
#include <QTextList>

std::unique_ptr<GlobalMementoBuilder> GlobalMementoBuilder::_instance;

GlobalMementoBuilder::GlobalMementoBuilder(QTextEdit* editor)
	: m_editor{editor}
{	}

bool GlobalMementoBuilder::supportAction(ActionType action) const
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
	case ActionType::FormatChecked:
    case ActionType::TextChange:
		return true;
	default:
		return false;
	}
}

//TODO enable support other action mementos
MementoUP GlobalMementoBuilder::buildMemento(QByteArray&& data, ActionType action)
{
	MementoUP memento;

	switch (action)
	{
    case ActionType::FormatBold:
        memento.reset(new BoldMemento);
        break;
	case ActionType::FormatItalic:
        memento.reset(new ItalicMemento);
        break;
	case ActionType::FormatUnderline:
        memento.reset(new UnderlineMemento);
        break;
	case ActionType::FormatAlignLeft:
        memento.reset(new AlignLeftMemento);
        break;
	case ActionType::FormatAlignRight:
        memento.reset(new AlignRightMemento);
        break;
	case ActionType::FormatAlignCenter:
        memento.reset(new AlignCenterMemento);
        break;
	case ActionType::FormatAlignJustify:
        memento.reset(new AlignJustifyMemento);
        break;
	case ActionType::FormatIndent:
        //memento.reset()
        break;
	case ActionType::FormatUnindent:
	case ActionType::FormatColor:
        memento.reset(new ColorMemento);
        break;
	case ActionType::FormatUnderlineColor:
        memento.reset(new UnderlineColorMemento);
        break;
    case ActionType::FormatChecked:
        break;
    case ActionType::FontSize:
        memento.reset(new SizeMemento);
        break;
    case ActionType::FontFamily:
        memento.reset(new FamilyMemento);
        break;
    case ActionType::TextChange:
        memento.reset(new TextChangeMemento);
        break;
    default:
        auto errorMsg = QString{"%1: Attemption build memento from unsupported ActionType{%2}"}
                .arg(FUNC_SIGN)
                .arg(static_cast<int>(action));

        throw std::logic_error{errorMsg.toStdString()};
	}

    memento->initFromRaw(std::move(data));

    return memento;
}

ActionUP GlobalMementoBuilder::buildAction(MementoUP memento)
{
    ActionUP action;

    switch(memento->getActionType())
    {
    case ActionType::FormatBold:
    {
        auto o_action = new FormatBold{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatItalic:
    {
        auto o_action = new FormatItalic{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatUnderline:
    {
        auto o_action = new FormatUnderline{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatAlignLeft:
    {
        auto o_action = new FormatAlignLeft{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatAlignRight:
    {
        auto o_action = new FormatAlignRight{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatAlignCenter:
    {
        auto o_action = new FormatAlignCenter{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatAlignJustify:
    {
        auto o_action = new FormatAlignJustify{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatIndent:
    case ActionType::FormatUnindent:
    {
        auto o_action = new FormatIndent{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatColor:
    {
        auto o_action = new FormatColor{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatUnderlineColor:
    {
        auto o_action = new FormatColor{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FormatChecked:
    {
        auto o_action = new FormatChecked{m_editor};
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FontSize:
    {
        auto o_action = new FormatSize{ m_editor };
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::FontFamily:
    {
        auto o_action = new FormatFamily{ m_editor };
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    case ActionType::TextChange:
    {
        auto o_action = new TextChangeAction{ m_editor };
        o_action->setMemento(std::move(memento));
        action.reset(o_action);
        break;
    }
    default:
        auto errorMsg = QString{"%1: Attemption build action from unsupported ActionType{%2}"}
                .arg(FUNC_SIGN)
                .arg(static_cast<int>(memento->getActionType()));

        throw std::logic_error{errorMsg.toStdString()};
    }

    return action;
}

void GlobalMementoBuilder::createInstance(QTextEdit* editor)
{
    auto builder = new GlobalMementoBuilder{ editor };
    GLOBAL_BUILDERS << builder;
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
	auto cursor = m_editor->textCursor();

	if (!cursor.hasSelection())
	{
		cursor.select(QTextCursor::WordUnderCursor);
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
    if (cursor.currentList()) {
        QTextListFormat listFmt = cursor.currentList()->format();
        // See whether the line above is the list we want to move this item into,
        // or whether we need a new list.
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() && listFmt.indent() + indent == above.currentList()->format().indent()) {
            above.currentList()->add(cursor.block());
        }
        else {
            listFmt.setIndent(listFmt.indent() + indent);
            cursor.createList(listFmt);
        }
    }
    else {
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
