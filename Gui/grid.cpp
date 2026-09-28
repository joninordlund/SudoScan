#include "grid.h"
#include "cellchange.h"
#include "sudokusolver.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <qtimer.h>
#include <solutionset.h>

GridView::GridView(QWidget* parent) :
    QWidget(parent),
    m_history(this),
    m_cells(81),
    m_lastSelectedCell(nullptr),
    m_isDragging(false),
    m_isPeeking(false),
    m_dirty(false),
    m_editMode(SOLVE),
    m_removedCount(33),
    m_solLocked(false)
{
    setFixedSize(630, 630);
    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 81; i++)
    {
        int row = i / 9;
        int col = i % 9;
        m_cells[i] = new Cell(row, col, this);
        connect(m_cells[i], &Cell::cellClicked, this, &GridView::handleMousePress);
        layout->addWidget(m_cells[i], row, col);
    }
    connect(&m_history, &History::historyStateChanged, this, &GridView::handleHistoryChanged);

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(150);

    connect(m_debounceTimer, &QTimer::timeout, this, [this]()
            {
        Matrix currentBoard = m_board.toIntMatrix();
        int maxCount = m_solutionSet.maxCount();
        
        (void)QtConcurrent::run([this, currentBoard, maxCount]() {
            m_solutionSet.updateSolutions(currentBoard, maxCount);
            QMetaObject::invokeMethod(this, [this]() {
                emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
            }, Qt::QueuedConnection);
        }); });
    updateUI();
    setFocusPolicy(Qt::StrongFocus);
}

void GridView::updateUI()
{
    for (Cell* cell : m_cells)
    {
        CellData data = m_board.data(cell->row(), cell->col());
        cell->setDigit(data.digit, data.isGiven);
        cell->setCenterMarks(data.centerMarks);
        cell->setCornerMarks(data.cornerMarks);
    }
}

void GridView::applyStateChange(int x, int y, const CellData& newData)
{
    Cell* cell = m_cells[x * 9 + y];
    m_board.setCellData(x, y, newData);
    cell->setDigit(newData.digit, newData.isGiven);
    cell->setCornerMarks(newData.cornerMarks);
    cell->setCenterMarks(newData.centerMarks);
    cell->update();

    qDebug() << "Applied state at (" << x << "," << y << ") digit:" << newData.digit
             << "Is grid full now?" << m_board.isGridFull();
}

void GridView::newSudoku(const reader::SudokuGrid& grid)
{
    m_board.setGivens(grid);
    m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
    updateUI();
}

void GridView::handleMousePress(Cell* cell, bool ctrl)
{
    m_isDragging = true;
    if (ctrl)
    {
        if (m_selectedCellSet.contains(cell))
        {
            m_selectedCellSet.remove(cell);
            cell->setSelected(false);
        }
        else
        {
            m_selectedCellSet.insert(cell);
            cell->setSelected(true);
        }
    }
    else
    {
        for (Cell* cell : as_const(m_selectedCellSet))
        {
            cell->setSelected(false);
        }
        m_selectedCellSet.clear();
        m_selectedCellSet.insert(cell);
        cell->setSelected(true);
    }
    if (m_lastSelectedCell)
    {
        m_lastSelectedCell->setCursor(false);
    }

    m_lastSelectedCell = cell;
    m_lastSelectedCell->setCursor(true);
}

void GridView::mouseReleaseEvent(QMouseEvent* event)
{
    m_isDragging = false;
}

void GridView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && event->buttons() & Qt::LeftButton)
    {
        Cell* cell = dynamic_cast<Cell*>(childAt(event->pos()));
        if (cell)
        {
            m_selectedCellSet.insert(cell);
            m_lastSelectedCell->setCursor(false);
            cell->setSelected(true);
            cell->setCursor(true);
            m_lastSelectedCell = cell;
        }
    }
}

void GridView::keyPressEvent(QKeyEvent* event)
{
    if (m_isPeeking)
    {
        event->ignore();
        return;
    }

    bool alt = event->modifiers() & Qt::AltModifier; // corner pencil marks
    bool shift = event->modifiers() & Qt::ShiftModifier;  // multi-selection
    bool ctrl = event->modifiers() & Qt::ControlModifier; // center pencil marks

    int key = event->key();

    int digit = -1;

    if (key >= Qt::Key_1 && key <= Qt::Key_9)
    {
        digit = key - Qt::Key_1 + 1;
    }
    if (digit != -1)
    {
        EMarkType type = ctrl ? CENTERMARK : (alt ? CORNERMARK : DIGIT);
        enterDigit(digit, type);
        return;
    }

    if (key == Qt::Key_Delete || key == Qt::Key_Backspace)
    {
        deleteCell();
        return;
    }
    if (key == Qt::Key_Escape)
    {
        for (Cell* cell : as_const(m_selectedCellSet))
        {
            cell->setSelected(false);
            cell->setCursor(false);
        }
        m_lastSelectedCell = nullptr;
        return;
    }

    if (m_lastSelectedCell)
    {
        int row, col, newRow, newCol;
        row = newRow = m_lastSelectedCell->row();
        col = newCol = m_lastSelectedCell->col();

        switch (key)
        {
            case Qt::Key_Left:
                newCol = std::max(0, col - 1);
                break;
            case Qt::Key_Right:
                newCol = std::min(8, col + 1);
                break;
            case Qt::Key_Up:
                newRow = std::max(0, row - 1);
                break;
            case Qt::Key_Down:
                newRow = std::min(8, row + 1);
                break;
            default:
                return;
        }

        Cell* nextCell = m_cells[newRow * 9 + newCol];

        if (shift || ctrl)
        {
            m_selectedCellSet.insert(nextCell);
            nextCell->setSelected(true);
        }
        else
        {
            for (Cell* cell : as_const(m_selectedCellSet))
            {
                cell->setSelected(false);
            }
            m_selectedCellSet.clear();
            m_selectedCellSet.insert(nextCell);
            nextCell->setSelected(true);
        }
        m_lastSelectedCell->setCursor(false);
        m_lastSelectedCell = nextCell;
        m_lastSelectedCell->setCursor(true);
    }
}

void GridView::onShowSolution()
{
    m_isPeeking = true;
    m_board.saveData();

    if (m_solLocked)
    {
        resetSolutions();
    }
    else
    {
        peekSingleSolution();
    }
    updateUI();
}

void GridView::peekSingleSolution()
{
    m_isPeeking = true;
    m_board.saveData();

    vector<Matrix> sols = m_solutionSet.getRandomSudokuSet(m_board.toIntMatrix(), 1);

    if (!sols.empty())
    {
        m_board.applySolution(sols[0]);
    }
    updateUI();
}

void GridView::resetSolutions()
{
    m_board.clearUserDigits();
    m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());

    if (m_solutionSet.count() > 0)
    {
        m_board.applySolution(m_solutionSet.next());
    }
    else
    {
        m_board.clearUserDigits();
    }
    updateUI();
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
}

void GridView::onHideSolution()
{
    m_isPeeking = false;
    m_board.restoreData();
    updateUI();
}

void GridView::onEditModeChanged(bool checked)
{
    m_editMode = checked ? EDIT_CLUES : SOLVE;
    m_history.clear();
    updateGridFullStatus();
    m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
}

void GridView::onClearSolution()
{
    QString text;
    QString info;
    if (m_editMode == SOLVE)
    {
        text = "You are about to clear all but the clues. Are you sure?";
        info = "If you want to clear the clues also, switch the entering clues on.";
    }
    else
    {
        text = "You are about to clear everything. Are you sure?";
        info = "If you don't want to clear the clues, switch the solving mode on.";
    }
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setInformativeText(info);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok)
    {
        if (m_editMode == SOLVE)
        {
            m_board.clearUserDigits();
        }
        else
        {
            m_board.clearAll();
        }
        updateUI();
    }
}

void GridView::onRandom()
{
    Matrix empty(9, vector<int>(9, 0));
    vector<Matrix> random = m_solutionSet.getRandomSudokuSet(empty, 1);
    m_board.clearAll();
    newSudoku(random[0]);
}

void GridView::handleHistoryChanged()
{
    // m_debounceTimer->start();
    // m_solutionSet.updateSolutions(m_board.toIntMatrix());
    // m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());
    // emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
    updateGridFullStatus();
}

void GridView::setDifficulty(int level)
{
    vector<int> removed{35, 42, 48, 52, 55};
    m_removedCount = removed[level - 1];
}

void GridView::onGenerate()
{
    Matrix generated = m_solutionSet.generate(m_removedCount, m_symmetry);
    m_board.clearAll();
    newSudoku(generated);
}

void GridView::onSolutionLocked(bool locked)
{
    m_solLocked = locked;
    // m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());
    if (locked)
    {
        m_solutionSet.updateSolutions(m_board.toIntMatrix(), m_solutionSet.maxCount());
        if (m_solutionSet.count() > 0)
        {
            m_board.saveData();
            m_board.applySolution(m_solutionSet.next());
            emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
            updateUI();
            return;
        }
    }
    onHideSolution();
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
}

void GridView::onBrowseSolLeft()
{
    if (m_solutionSet.count() == 0)
    {
        return;
    }
    Matrix prev = m_solutionSet.prev();
    m_board.applySolution(prev);
    updateUI();
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
}

void GridView::onBrowseSolRight()
{

    if (m_solutionSet.count() == 0)
    {
        return;
    }

    Matrix next = m_solutionSet.next();
    m_board.applySolution(next);
    updateUI();
    emit solutionCountChanged(m_solutionSet.currentIndex() + 1, m_solutionSet.count());
}

void GridView::enterDigit(int digit, EMarkType type)
{
    if (m_editMode == EDIT_CLUES)
    {
        type = DIGIT;
    }
    changeCell(digit, type, m_editMode == EDIT_CLUES);
}

void GridView::deleteCell()
{
    changeCell(0, DELETE, false);
}

void GridView::changeCell(int digit, EMarkType type, bool isGiven)
{
    vector<CellChange> command;
    bool digitChanged = false;
    for (Cell* cell : as_const(m_selectedCellSet))
    {
        if (!cell->isGiven() || m_editMode == EDIT_CLUES)
        {
            int x = cell->row();
            int y = cell->col();
            CellData oldState = m_board.data(x, y);
            CellData newState = handleNumberInput(x, y, digit, type);
            if (oldState == newState)
            {
                continue;
            }
            if (oldState.digit != newState.digit)
            {
                digitChanged = true;
            }
            applyStateChange(x, y, newState);
            command.push_back(CellChange{x, y, oldState, newState});
        }
    }
    if (!command.empty())
    {
        m_history.setNewCommand(command);
    }
    if (digitChanged && m_editMode != SOLVE)
    {
        m_debounceTimer->start();
    }
    updateGridFullStatus();
}

CellData GridView::handleNumberInput(int x, int y, int digit, EMarkType type)
{
    CellData newState = m_board.data(x, y);
    switch (type)
    {
        case DIGIT:
            qDebug() << "Digit:" << digit;
            newState.isGiven = (m_editMode == EDIT_CLUES);
            newState.digit = (newState.digit == digit && !newState.isGiven) ? 0 : digit;
            break;
        case DELETE:
            newState.digit = 0;
            newState.isGiven = false;
            newState.cornerMarks = 0;
            newState.centerMarks = 0;
            break;
        case CORNERMARK:
            if (newState.digit == 0)
            {
                newState.cornerMarks ^= (1 << (digit - 1));
            }
            break;
        case CENTERMARK:
            if (newState.digit == 0)
            {
                newState.centerMarks ^= (1 << (digit - 1));
            }
            break;
    };
    return newState;
}

void GridView::updateGridFullStatus()
{
    bool full = (m_editMode == SOLVE) && m_board.isGridFull();
    emit gridFullStatusChanged(full);
}

