#pragma once

#include <memory>
#include <QByteArray>

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

    FontSize,
    FontFamily,

    EditCopy,
    EditPaste,
    EditCut,
    EditRedo,
    EditUndo,

    FileOpen,
    FileSave,
    FileSaveAs,

    DocNew,
    
    TextChange,
};

struct Memento;
struct Action;

using MementoUP = std::unique_ptr<Memento>;
using ActionUP = std::unique_ptr<Action>;

struct MementoDeserializer
{
    virtual ~MementoDeserializer() = default;
    virtual bool actionIsSupported(ActionType action) const = 0;
    virtual ActionUP deserializeAction(QByteArray&& data, ActionType action) = 0;
};

struct Action
{
    virtual ~Action() = default;
    virtual void execute() = 0;
    virtual const Memento* getMemento() const = 0;

protected:
    virtual void setMemento(MementoUP memento) = 0;
};

struct Memento
{
    virtual ~Memento() = default;
    virtual ActionType getActionType() const = 0;
    virtual QByteArray toRaw() const = 0;
    virtual void initFromRaw(QByteArray&& raw) = 0;
};
