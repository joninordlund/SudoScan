#include <chrono>
#include <vector>

#include "sudokusolver.h"
#include "DLXBuilder.h"

pair<Matrix, vector<tuple<int, int, int>>> SudokuSolver::makeSudokuMatrix(const Matrix &sudoku)
{
    const int M = 9;

    Matrix mat;
    mat.reserve(M * M * M);

    vector<tuple<int, int, int>> rowMapping;

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            for (int n = 1; n <= M; n++)
            {
                vector<int> row;
                row.resize(4 * M * M);
                if (sudoku[i][j] == 0 || sudoku[i][j] == n)
                {
                    // constraints
                    int box = (i / 3) * 3 + (j / 3);
                    row[i * M + j] = 1;                     // cell
                    row[M * M + i * M + (n - 1)] = 1;       // row
                    row[2 * M * M + j * M + (n - 1)] = 1;   // column
                    row[3 * M * M + box * M + (n - 1)] = 1; // box

                    mat.push_back(std::move(row));
                    rowMapping.emplace_back(i, j, n);
                }
            }
        }
    }
    auto res = make_pair(mat, rowMapping);
    return res;
}

vector<Matrix> SudokuSolver::getSolutions(const Matrix &sudoku, bool random, int maxCount)
{
    auto sudokuMat = makeSudokuMatrix(sudoku);

    DLXBuilder dlx(sudokuMat.first);
    if(random)
    {
        dlx.setRandomMode(true);
    }

    auto solutions = dlx.findSolutions(maxCount);

    Matrix solved(9, vector<int>(9, 0));
    vector<Matrix> res;
    for (int r = 0; r < solutions.size(); r++)
    {
        for (int rowID : solutions[r])
        {
            auto [i, j, n] = sudokuMat.second[rowID - 1];
            solved[i][j] = n;
        }
        res.push_back(solved);
    }
    dlx.setRandomMode(false);
    return res;
}

vector<Matrix> SudokuSolver::getRandomSudokuSet(const Matrix& sudoku, int count)
{
    vector<Matrix> results = getSolutions(sudoku, true, count);
    return results;
}
