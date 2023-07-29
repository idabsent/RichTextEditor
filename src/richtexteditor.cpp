#include "richtexteditor.hpp"
#include "menubarbuilder.hpp"

#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <algorithm>
#include <iterator>

RichTextEditor::RichTextEditor(QMainWindow* parent)
    : m_textEditor{nullptr}
{   }

void RichTextEditor::buildUi()
{
    setupFileActions();
    setupEditActions();
    setupFormatActions();
    setupFontSelectorToolBar();

    buildEditor();
}

void RichTextEditor::setupFileActions()
{
    MenuBarBuilder builder{this, tr("File actions"), tr("File")};

    auto newAction = builder.setActionShortcut(QKeySequence::New)
        .setActionIcon(QIcon{":/icons/filenew.png"})
        .createAction(tr("&New"));

    auto openAction = builder.setActionShortcut(QKeySequence::Open)
        .setActionIcon(QIcon{":/icons/fileopen.png"})
        .createAction(tr("&Open"));

    auto saveAction = builder.setActionShortcut(QKeySequence::Save)
        .setActionIcon(QIcon{":/icons/filesave.png"})
        .enableSepartorToMenu()
        .createAction(tr("&Save"));

    auto saveAsAction = builder.setActionShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S)
        .disableForToolBar()
        .createAction(tr("Save as"));

    auto quitAction = builder.setActionShortcut(QKeySequence::Quit)
        .disableForToolBar()
        .enableSepartorToMenu()
        .createAction(tr("Quit"));
}

void RichTextEditor::setupEditActions()
{
    MenuBarBuilder builder{ this, tr("Edit actions"), tr("Edit") };

    auto undoAction = builder.setActionShortcut(QKeySequence::Undo)
        .setActionIcon(QIcon{":/icons/editundo.png"})
        .createAction(tr("&Undo"));

    auto redoAction = builder.setActionShortcut(QKeySequence::Redo)
        .setActionIcon(QIcon{":/icons/editredo.png"})
        .createAction(tr("&Redo"));

    auto copyAction = builder.setActionShortcut(QKeySequence::Copy)
        .setActionIcon(QIcon{":/icons/editcopy.png"})
        .enableSepartorToMenu()
        .createAction(tr("&Copy"));

    auto cutAction = builder.setActionShortcut(QKeySequence::Cut)
        .setActionIcon(QIcon{":/icons/editcut.png"})
        .createAction(tr("&Cut"));

    auto pasteAction = builder.setActionShortcut(QKeySequence::Paste)
        .setActionIcon(QIcon{":/icons/editpaste.png"})
        .createAction(tr("&Paste"));
}

void RichTextEditor::setupFormatActions()
{
    MenuBarBuilder builder{ this, tr("Format actions"), tr("Format") };

    auto boldAction = builder.setActionShortcut(QKeySequence::Bold)
        .setActionIcon(QIcon{":/icons/formatbold.png"})
        .createAction(tr("&Bold"));

    auto italicAction = builder.setActionShortcut(QKeySequence::Italic)
        .setActionIcon(QIcon{":/icons/formatitalic.png"})
        .createAction(tr("&Italic"));

    auto underlineAction = builder.setActionShortcut(QKeySequence::Underline)
        .setActionIcon(QIcon{":/icons/formatunderline.png"})
        .enableSepartorToMenu()
        .createAction(tr("&Underline"));

    auto aligntLeftAction = builder.setActionShortcut(Qt::ALT | Qt::Key_L)
        .setActionIcon(QIcon{":/icons/formatalignleft.png"})
        .createAction(tr("Align &Left"));

    auto alignCenterAction = builder.setActionShortcut(Qt::ALT | Qt::Key_C)
        .setActionIcon(QIcon{":/icons/formataligncenter.png"})
        .createAction(tr("Align &Center"));

    auto alignRightAction = builder.setActionShortcut(Qt::ALT | Qt::Key_R)
        .setActionIcon(QIcon{":/icons/formatalignright.png"})
        .createAction(tr("Algin &Right"));

    auto alignJustifyAction = builder.setActionShortcut(Qt::ALT | Qt::Key_J)
        .setActionIcon(QIcon{":/icons/formatalignjustify.png"})
        .enableSepartorToMenu()
        .createAction(tr("Align &Justify"));

    auto indentAction = builder.setActionShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_I)
        .setActionIcon(QIcon{":/icons/formatindent.png"})
        .createAction(tr("Indent"));

    auto unindentAction = builder.setActionShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_U)
        .setActionIcon(QIcon{":/icons/formatunindent.png"})
        .createAction(tr("Unindent"));
}

void RichTextEditor::setupFontSelectorToolBar()
{
    auto fontSelectorToolBar = addToolBar(tr("Font selector"));
    
    auto styleSelector = new QComboBox;
    auto sizeSelector = new QComboBox;
    auto fontSelector = new QFontComboBox;

    styleSelector->addItems({
        "Standard",
        "Bullet List(Disc)",
        "Bullet List(Circle)",
        "Bullet List(Square)",
        "Task List(Unchecked)",
        "Task List(Checked)",
        "Ordered list(Decimal)",
        "Ordered list(Alpha upper)",
        "Ordered list(Alpha lower)",
        "Ordered list(Roman upper)",
        "Ordered list(Roman lower)",
        "Heading 1",
        "Heading 2",
        "Heading 3",
        "Heading 4",
        "Heading 5",
        "Heading 6"
        }
    );

    auto sizes = QFontDatabase::standardSizes();
    QStringList sSizes;
    for (auto size : sizes)
    {
        sSizes << QString::number(size);
    }

    sizeSelector->addItems(sSizes);

    fontSelectorToolBar->addWidget(styleSelector);
    fontSelectorToolBar->addWidget(fontSelector);

}

void RichTextEditor::buildEditor()
{
    m_textEditor = new QTextEdit;
    setCentralWidget(m_textEditor);
}
