#include "solutionset.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

void SolutionSet::updateSolutions(const Matrix& matrix, int maxCount)
{
    m_maxSolCount = maxCount;
    m_solutions = getRandomSudokuSet(matrix, maxCount);
    m_index = -1;
}

vector<Matrix> SolutionSet::getRandomSudokuSet(const Matrix& sudoku, int count)
{
    return m_solver.getRandomSudokuSet(sudoku, count);
}

bool SolutionSet::isUnique(const Matrix& matrix)
{
    auto sols = m_solver.getSolutions(matrix, false, 2);
    return sols.size() == 1;
}

Matrix SolutionSet::next()
{
    if (m_solutions.empty())
    {
        return Matrix();
    }

    m_index = (m_index + 1) % m_solutions.size();
    // cout << "next index: " << m_index << endl;
    return m_solutions[m_index];
}

Matrix SolutionSet::prev()
{
    if (m_solutions.empty())
    {
        return Matrix();
    }

    if (m_index <= 0)
    {
        m_index = m_solutions.size() - 1;
    }
    else
    {
        m_index--;
    }
    // cout << "prev index: " << m_index << endl;
    return m_solutions[m_index];
}

namespace
{
std::pair<int, int> getSymmetricCell(int r, int c, Symmetry mode)
{
    switch (mode)
    {
        case Symmetry::Rotate180:
            return {8 - r, 8 - c};
        case Symmetry::Vertical:
            return {r, 8 - c};
        case Symmetry::Horizontal:
            return {8 - r, c};
        case Symmetry::None:
            return {r, c};
    }
    return {r, c};
}
}

Matrix SolutionSet::generate(int target, Symmetry symmetry = Symmetry::Rotate180)
{
    Matrix zeroMatrix(9, vector<int>(9, 0));
    vector<Matrix> set = getRandomSudokuSet(zeroMatrix, 10);

    Matrix bestResult;
    int maxRemoved = 0;
    std::mt19937 rndGen(std::random_device{}());

    for (Matrix grid : set)
    {
        std::vector<int> cells(81);
        std::iota(cells.begin(), cells.end(), 0);
        std::shuffle(cells.begin(), cells.end(), rndGen);

        int removedCount = 0;

        for (int cellIdx : cells)
        {
            int r = cellIdx / 9;
            int c = cellIdx % 9;

            if (grid[r][c] == 0)
            {
                continue;
            }

            auto [r2, c2] = getSymmetricCell(r, c, symmetry);

            int backup = grid[r][c];
            int backup2 = grid[r2][c2];

            grid[r][c] = 0;
            grid[r2][c2] = 0;

            if (isUnique(grid))
            {
                if (r == r2 && c == c2)
                {
                    removedCount += 1;
                }
                else
                {
                    removedCount += 2;
                }

                if (removedCount > maxRemoved)
                {
                    maxRemoved = removedCount;
                    bestResult = grid;
                }
            }
            else
            {
                grid[r][c] = backup;
                grid[r2][c2] = backup2;
            }

            if (removedCount >= target)
            {
                std::cout << "POISTETTU: " << maxRemoved << std::endl;
                return grid;
            }
        }
    }
    std::cout << "POISTETTU: " << maxRemoved << std::endl;
    return bestResult;
}
