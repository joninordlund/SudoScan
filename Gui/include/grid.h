#pragma once

#include "cell.h"
#include "history.h"
#include "solutionset.h"
#include "sudokuboard.h"
#include "sudokureader.h"

#include <QFuture>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <QtConcurrent>

enum EEditMode {
    SOLVE = 1,
    EDIT_CLUES
};

enum EMarkType {
    DIGIT = 1,
    DELETE,
    CENTERMARK,
    CORNERMARK
};

using namespace std;
// using SudokuGrid = QVector<Cell*>;

class GridView : public QWidget {
    Q_OBJECT

  public:
    explicit GridView(int cellSize, QWidget* parent = nullptr);

    // vector<vector<CellData>> toMatrix();
    // void fromMatrix(const vector<vector<CellData>>& matrix);
    void applyStateChange(int x, int y, const CellData& data);
    History* getHistory() { return &m_history; }
    SudokuBoard& getBoard() { return m_board; }
    void updateUI();
    void setDifficulty(int level);
    void setSymmetry(Symmetry symmetry) { m_symmetry = symmetry; }

  public slots:
    void newSudoku(const reader::SudokuGrid& grid);
    void handleMousePress(Cell* cell, bool ctrl);
    void onShowSolution();
    void onHideSolution();
    void onEditModeChanged(bool checked);
    void onClearSolution();
    void onRandom();
    void onUndo() {
        m_history.undo();
        updateGridFullStatus();
    }
    void onRedo() {
        m_history.redo();
        updateGridFullStatus();
    }
    void handleHistoryChanged();
    void onGenerate();
    void onSolutionLocked(bool locked);
    void onBrowseSolLeft();
    void onBrowseSolRight();

  signals:
    void solutionCountChanged(int count, int maxCount);
    void gridFullStatusChanged(bool isFull);

  private:
    QVector<Cell*> m_cells;
    QSet<Cell*> m_selectedCellSet;
    Cell* m_lastSelectedCell;
    bool m_isDragging;
    bool m_isPeeking;
    EEditMode m_editMode;
    SudokuBoard m_board;
    SolutionSet m_solutionSet;
    History m_history;
    int m_removedCount;
    bool m_solLocked;
    Symmetry m_symmetry = Symmetry::None;
    QTimer* m_debounceTimer;

    void enterDigit(int digit, EMarkType type);
    void deleteCell();
    void changeCell(int digit, EMarkType type, bool isGiven);
    CellData handleNumberInput(int x, int y, int digit, EMarkType inputType);
    void resetSolutions();
    void peekSingleSolution();

    void updateGridFullStatus();

  protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};
