#include <vector>
#include "DLXBuilder.h"
#include "sudokusolver.h"

pair<vector<SparseRow>, vector<tuple<int, int, int>>> SudokuSolver::makeSudokuMatrix(const Matrix& sudoku)
{
    const int M = 9;
    vector<SparseRow> rows;
    vector<tuple<int, int, int>> rowMapping;
    rows.reserve(M * M * M);
    rowMapping.reserve(M * M * M);

    for (int i = 0; i < M; i++)
        for (int j = 0; j < M; j++)
            for (int n = 1; n <= M; n++)
            {
                if (sudoku[i][j] != 0 && sudoku[i][j] != n)
                {
                    continue;
                }
                int box = (i / 3) * 3 + (j / 3);
                rows.push_back({i * M + j,                       // cell
                                M * M + i * M + (n - 1),         // row
                                2 * M * M + j * M + (n - 1),     // column
                                3 * M * M + box * M + (n - 1)}); // box
                rowMapping.emplace_back(i, j, n);
            }
    return {std::move(rows), std::move(rowMapping)};
}

vector<Matrix> SudokuSolver::getSolutions(const Matrix& sudoku, bool random, int maxCount)
{
    auto [rows, mapping] = makeSudokuMatrix(sudoku);

    DLXBuilder dlx(4 * 81, std::move(rows), maxCount);
    dlx.setRandomMode(random);

    auto solutions = dlx.findSolutions(maxCount);

    vector<Matrix> res;
    res.reserve(solutions.size());
    for (const auto& sol : solutions)
    {
        Matrix solved(9, vector<int>(9, 0));
        for (int rowID : sol)
        {
            auto [i, j, n] = mapping[rowID - 1];
            solved[i][j] = n;
        }
        res.push_back(std::move(solved));
    }
    return res;
}
