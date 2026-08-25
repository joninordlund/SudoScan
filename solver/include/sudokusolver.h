#pragma once

#include <vector>

using namespace std;
using Matrix = vector<vector<int>>;

class SudokuSolver
{
    friend class SolutionSet;
    SudokuSolver() {}
    pair<Matrix, vector<tuple<int, int, int>>> makeSudokuMatrix(const Matrix &sudoku);
    vector<Matrix> getSolutions(const Matrix &grid, bool random, int maxCount);
    vector<Matrix> getRandomSudokuSet(const Matrix& sudoku, int count);
};
