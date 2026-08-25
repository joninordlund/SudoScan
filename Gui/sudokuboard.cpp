#include "sudokuboard.h"
#include <QDebug>

SudokuBoard::SudokuBoard() :
    m_data(9, vector<CellData>(9, CellData{0, false}))
{
}

void SudokuBoard::setCenterMarks(int row, int col, int value)
{
    if (digit(row, col) == 0)
    {
        uint16_t centerMarks = m_data[row][col].centerMarks;
        m_data[row][col] = CellData{0, false, centerMarks ^= (1 << (value - 1)), 0};
        qDebug() << centerMarks;
    }
}

void SudokuBoard::clearUserDigits()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (!m_data[i][j].isGiven)
            {
                m_data[i][j].digit = 0;
                m_data[i][j].cornerMarks = 0;
                m_data[i][j].centerMarks = 0;
            }
        }
    }
}

void SudokuBoard::clearAll()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            m_data[i][j] = CellData{0, false, 0, 0};
        }
    }
}

Grid2D<int> SudokuBoard::toIntMatrix() const
{
    Grid2D<int> matrix(9, vector<int>(9, 0));
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            matrix[i][j] = m_data[i][j].digit;
        }
    }
    return matrix;
}

void SudokuBoard::applySolution(const Grid2D<int>& sol)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (!m_data[i][j].isGiven)
            {
                m_data[i][j].digit = sol[i][j];
                m_data[i][j].centerMarks = 0;
                m_data[i][j].cornerMarks = 0;
            }
        }
    }
}

void SudokuBoard::setGivens(const Grid2D<int>& clues)
{
    clearAll();
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (clues[i][j] > 0)
            {
                m_data[i][j].digit = clues[i][j];
                m_data[i][j].isGiven = true;
            }
        }
    }
}

namespace
{
bool isValidGroup(const vector<int>& group)
{
    int mask = 0;
    for (int digit : group)
    {
        int bit = 1 << (digit - 1);
        if ((mask & bit) != 0)
        {
            return false;
        }
        mask |= bit;
    }
    return mask == 0b111111111;
}
}

bool SudokuBoard::isSudokuSolutionValid()
{
    Grid2D<int> board = toIntMatrix();
    for (int i = 0; i < 9; i++)
    {
        if (!isValidGroup(board[i]))
        {
            return false;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        std::vector<int> column(9);
        for (int j = 0; j < 9; j++)
        {
            column[j] = board[j][i];
        }
        if (!isValidGroup(column))
        {
            return false;
        }
    }
    for (int boxRow = 0; boxRow < 9; boxRow += 3)
    {
        for (int boxCol = 0; boxCol < 9; boxCol += 3)
        {
            std::vector<int> box;
            box.reserve(9);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    box.push_back(board[boxRow + i][boxCol + j]);
                }
            }
            if (!isValidGroup(box))
            {
                return false;
            }
        }
    }
    return true;
}

bool SudokuBoard::isGridFull()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (m_data[i][j].digit == 0)
            {
                return false;
            }
        }
    }
    return true;
}
