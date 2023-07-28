#pragma once

#include <memory>

enum class ActionType : uint16_t
{
    FormatBold,
    FormatItalic,
    FormatUnderline,
    FormatAlignLeft,
    FormatAlignRight,
    FormatAlignCenter,
    FormatAlignJustify,
    FormatIndent,
    FormatUnindent,
    FormatColor,
    FormatUnderlineColor,
    FormatChecked,

    EditCopy,
    EditPaste,
    EditCut,
    EditRedo,
    EditUndo,

    FileSave,
    FileNew,
    FileOpen,
};

struct Memento;
struct Action;

using MementoUP = std::unique_ptr<Memento>;
using ActionUP = std::unique_ptr<Action>;

struct MementoBuilder
{
    virtual bool supportMemento(Memento* memento) = 0;
    virtual ActionUP buildAction(MementoUP memento) = 0;
};

struct Action
{
    virtual void redo() = 0;
    virtual void undo() = 0;
    virtual const Memento* getMemento() const = 0;

private:
    virtual void setMemento(MementoUP memento) = 0;

    friend class MementoBuilder;
};

struct Memento
{
    virtual ActionType getActionType() = 0;
};
