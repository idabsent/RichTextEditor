#include "richtexteditor.hpp"
#include "menubarbuilder.hpp"
#include "dbussession.hpp"
#include "formatactions.hpp"
#include "texteditoractions.hpp"

#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QDebug>
#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QApplication>

#include <algorithm>
#include <iterator>

RichTextEditor::RichTextEditor(QMainWindow* parent)
    : m_docsEditor{nullptr}
    , m_builder{new MenuBarBuilder{this}}
    , m_textObserver{nullptr}
{   }

void RichTextEditor::buildUi()
{
    setupFileActions();
    setupEditActions();
    setupFormatActions();
    setupFontSelectorToolBar();

    buildEditorAndObjects();
}

void RichTextEditor::setupFileActions()
{
    m_builder->startBuild(tr("File actions"), tr("File"));

    auto newFn = [&](QAction* action)
    {
        return new DocNewAction{m_docsEditor};
    };

    auto newAction = m_builder->setActionShortcut(QKeySequence::New)
        ->setActionIcon(QIcon{":/icons/filenew.png"})
        ->createAction(tr("&New"), newFn);

    auto openFn = [&](QAction* action)
    {
        auto path = QFileDialog::getOpenFileName(this);
        return path.isEmpty() ? nullptr : new FileOpenAction{path, m_docsEditor};
    };

    auto openAction = m_builder->setActionShortcut(QKeySequence::Open)
        ->setActionIcon(QIcon{":/icons/fileopen.png"})
        ->createAction(tr("&Open"), openFn);

    auto saveFn = [&](QAction* action)
    {
        QFileInfo info{m_docsEditor->getEditor()->documentTitle()};
        auto path = info.exists() ? info.absoluteFilePath() : QFileDialog::getSaveFileName(this);

        return path.isEmpty() ? nullptr : new FileSaveAction{path, m_docsEditor};
    };

    auto saveAction = m_builder->setActionShortcut(QKeySequence::Save)
        ->setActionIcon(QIcon{":/icons/filesave.png"})
        ->enableSepartorToMenu()
        ->createAction(tr("&Save"), saveFn);

    auto saveAsFn = [&](QAction* action)
    {
        auto path = QFileDialog::getSaveFileName(this);
        return path.isEmpty() ? nullptr : new FileSaveAsAction{path, m_docsEditor};
    };

    auto saveAsAction = m_builder->setActionShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S)
        ->disableForToolBar()
        ->createAction(tr("Save as"), saveAsFn);

    auto quitFn = [&](QAction* action)
    {
        QApplication::exit(EXIT_SUCCESS);
        return nullptr;
    };

    auto quitAction = m_builder->setActionShortcut(QKeySequence::Quit)
        ->disableForToolBar()
        ->enableSepartorToMenu()
        ->createAction(tr("Quit"), quitFn);

    m_builder->endBuild();
}

void RichTextEditor::setupEditActions()
{
    m_builder->startBuild(tr("Edit actions"), tr("Edit"));

    auto undoFn = [&](QAction* action)
    {
        return new EditUndoAction{m_docsEditor->getEditor()};
    };

    auto undoAction = m_builder->setActionShortcut(QKeySequence::Undo)
        ->setActionIcon(QIcon{":/icons/editundo.png"})
        ->createAction(tr("&Undo"), undoFn);

    auto redoFn = [&](QAction* action)
    {
        return new EditRedoAction{m_docsEditor->getEditor()};
    };

    auto redoAction = m_builder->setActionShortcut(QKeySequence::Redo)
        ->setActionIcon(QIcon{":/icons/editredo.png"})
        ->createAction(tr("&Redo"), redoFn);

    auto copyFn = [&](QAction* action)
    {
        return new EditCopyAction{m_docsEditor->getEditor()};
    };

    auto copyAction = m_builder->setActionShortcut(QKeySequence::Copy)
        ->setActionIcon(QIcon{":/icons/editcopy.png"})
        ->enableSepartorToMenu()
        ->createAction(tr("&Copy"), copyFn);

    auto cutFn = [&](QAction* action)
    {
        return new EditCutAction{m_docsEditor->getEditor()};
    };

    auto cutAction = m_builder->setActionShortcut(QKeySequence::Cut)
        ->setActionIcon(QIcon{":/icons/editcut.png"})
        ->createAction(tr("&Cut"), cutFn);

    auto pasteFn = [&](QAction* action)
    {
        return new EditPasteAction{m_docsEditor->getEditor()};
    };

    auto pasteAction = m_builder->setActionShortcut(QKeySequence::Paste)
        ->setActionIcon(QIcon{":/icons/editpaste.png"})
        ->createAction(tr("&Paste"), pasteFn);

    m_builder->endBuild();
}

void RichTextEditor::setupFormatActions()
{
    m_builder->startBuild(tr("Format actions"), tr("Format"));

    auto boldFn = [=](QAction* qAction)
    {
        return new FormatBold{ qAction->isChecked(), m_docsEditor->getEditor() };
    };

    auto boldAction = m_builder->setActionShortcut(QKeySequence::Bold)
        ->setActionIcon(QIcon{":/icons/formatbold.png"})
        ->setCheckable(true)
        ->createAction(tr("&Bold"), boldFn);

    auto italicFn = [=](QAction* qAction)
    {
        return new FormatItalic{ qAction->isChecked(), m_docsEditor->getEditor() };
    };

    auto italicAction = m_builder->setActionShortcut(QKeySequence::Italic)
        ->setActionIcon(QIcon{":/icons/formatitalic.png"})
        ->setCheckable(true)
        ->createAction(tr("&Italic"), italicFn);

    auto underlineFn = [=](QAction* qAction)
    {
        return new FormatUnderline{ qAction->isChecked(), m_docsEditor->getEditor() };
    };

    auto underlineAction = m_builder->setActionShortcut(QKeySequence::Underline)
        ->setActionIcon(QIcon{":/icons/formatunderline.png"})
        ->setCheckable(true)
        ->enableSepartorToMenu()
        ->createAction(tr("&Underline"), underlineFn);

    auto alignLeftFn = [=](QAction* action)
    {
        return new FormatAlignLeft{ m_docsEditor->getEditor() };
    };

    auto aligntLeftAction = m_builder->setActionShortcut(Qt::ALT | Qt::Key_L)
        ->setActionIcon(QIcon{":/icons/formatalignleft.png"})
        ->createAction(tr("Align &Left"), alignLeftFn);

    auto alignCenterFn = [=](QAction* action)
    {
        return new FormatAlignCenter{ m_docsEditor->getEditor() };
    };

    auto alignCenterAction = m_builder->setActionShortcut(Qt::ALT | Qt::Key_C)
        ->setActionIcon(QIcon{":/icons/formataligncenter.png"})
        ->createAction(tr("Align &Center"), alignCenterFn);

    auto alignRightFn = [=](QAction* action)
    {
        return new FormatAlignRight{ m_docsEditor->getEditor() };
    };

    auto alignRightAction = m_builder->setActionShortcut(Qt::ALT | Qt::Key_R)
        ->setActionIcon(QIcon{":/icons/formatalignright.png"})
        ->createAction(tr("Algin &Right"), alignRightFn);

    auto alignJustifyFn = [=](QAction* action)
    {
        return new FormatAlignJustify{ m_docsEditor->getEditor() };
    };

    auto alignJustifyAction = m_builder->setActionShortcut(Qt::ALT | Qt::Key_J)
        ->setActionIcon(QIcon{":/icons/formatalignjustify.png"})
        ->enableSepartorToMenu()
        ->createAction(tr("Align &Justify"), alignJustifyFn);

    auto indentFn = [=](QAction* action)
    {
        return new FormatIndent{ 1, m_docsEditor->getEditor() };
    };

    auto indentAction = m_builder->setActionShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_I)
        ->setActionIcon(QIcon{":/icons/formatindent.png"})
        ->createAction(tr("Indent"), indentFn);

    auto unindentFn = [=](QAction* action)
    {
        return new FormatIndent{ -1, m_docsEditor->getEditor() };
    };

    auto unindentAction = m_builder->setActionShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_U)
        ->setActionIcon(QIcon{":/icons/formatunindent.png"})
        ->createAction(tr("Unindent"), unindentFn);

    auto colorFn = [=](QAction* qAction)
    {
        auto color = QColorDialog::getColor(m_docsEditor->getEditor()->textColor(), this);
        return new FormatColor{ color, m_docsEditor->getEditor() };
    };

    auto colorAction = m_builder->setActionIcon(QIcon{ ":/icons/formatcolor.png" })
        ->createAction(tr("Change color"), colorFn);

    auto underlineColorFn = [=](QAction* indent)
    {
        auto color = QColorDialog::getColor(m_docsEditor->getEditor()->textBackgroundColor(), m_docsEditor->getEditor());
        return new FormatUnderlineColor{ color, m_docsEditor->getEditor() };
    };

    auto underlineColorAction = m_builder->setActionIcon(QIcon{ ":/icons/formatunderlinecolor.png" })
        ->createAction(tr("Change underline color"));

    m_builder->endBuild();
}

void RichTextEditor::setupFontSelectorToolBar()
{
    auto fontSelectorToolBar = addToolBar(tr("Font selector"));

    auto sizeSelector = new QComboBox;
    auto fontSelector = new QFontComboBox;

    auto sizes = QFontDatabase::standardSizes();
    QStringList sSizes;
    for (auto size : sizes)
    {
        sSizes << QString::number(size);
    }

    sizeSelector->addItems(sSizes);

    connect(sizeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=] (int index)
        {
            auto size = sizeSelector->itemText(index).toInt();
            auto action = std::make_unique<FormatSize>(size, m_docsEditor->getEditor());
            action->execute();
        }
    );

    connect(fontSelector, &QFontComboBox::currentFontChanged, this, [=](QFont const& font)
        {
            auto action = std::make_unique<FormatFamily>(font.family(), m_docsEditor->getEditor());
            action->execute();
        }
    );

    fontSelectorToolBar->addWidget(fontSelector);
    fontSelectorToolBar->addWidget(sizeSelector);
}

void RichTextEditor::buildEditorAndObjects()
{
    auto editor = new AdditionalEmiterTextEditor;
    m_docsEditor = new EditorTabWidget{ editor };
    GlobalMementoBuilder::createInstance(m_docsEditor);
    m_actionsObserver = new DBusActionsObserver{m_docsEditor->getEditor(), this};editor->setDocumentTitle("Example title");
    setCentralWidget(m_docsEditor);
    m_textObserver = new TextChangeObserver{ editor, 10 };
}
