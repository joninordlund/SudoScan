#pragma once
#include "cellchange.h"
#include <vector>

using namespace std;

template <typename T>
using Grid2D = std::vector<std::vector<T>>;

class SudokuBoard
{
  public:
    SudokuBoard();
    SudokuBoard(const Grid2D<int>& puzzle);

    CellData data(int row, int col) const { return m_data[row][col]; }
    int digit(int row, int col) const { return m_data[row][col].digit; }
    bool isGiven(int row, int col) const { return m_data[row][col].isGiven; }

    void setCellData(int row, int col, CellData data) { m_data[row][col] = data; }
    void setDigit(int row, int col, int digit, bool given) { m_data[row][col] = CellData{digit, given, 0, 0}; }
    void setCenterMarks(int row, int col, int digit);
    void clearUserDigits();
    void clearAll();

    Grid2D<int> toIntMatrix() const;
    Grid2D<CellData> toCellDataMatrix() const { return m_data; }

    void saveData() { m_snapShot = m_data; }
    void restoreData() { m_data = m_snapShot; }

    void applySolution(const Grid2D<int>& sol);

    void setGivens(const Grid2D<int>& clues);
    bool isSudokuSolutionValid(const Grid2D<int>& board);

  private:
    Grid2D<CellData> m_data;
    Grid2D<CellData> m_snapShot;
};
