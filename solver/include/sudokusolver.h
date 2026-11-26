// #pragma once

// #include <vector>

// using namespace std;
// using Matrix = vector<vector<int>>;

// class SudokuSolver
// {
//     friend class SolutionSet;
//     SudokuSolver() {}
//     pair<Matrix, vector<tuple<int, int, int>>> makeSudokuMatrix(const Matrix &sudoku);
//     vector<Matrix> getSolutions(const Matrix &grid, bool random, int maxCount);
//     vector<Matrix> getRandomSudokuSet(const Matrix& sudoku, int count);
// };

#pragma once
#include <array>
#include <tuple>
#include <vector>

using namespace std;
using Matrix = vector<vector<int>>;
using SparseRow = array<int, 4>;

class SudokuSolver {
    friend class SolutionSet;
    SudokuSolver() {}
    pair<vector<SparseRow>, vector<tuple<int, int, int>>> makeSudokuMatrix(const Matrix& sudoku);
    vector<Matrix> getSolutions(const Matrix& grid, bool random, int maxCount);
    vector<Matrix> getRandomSudokuSet(const Matrix& sudoku, int count) { return getSolutions(sudoku, true, count); }
    vector<Matrix> getFirstSolutions(const Matrix& sudoku, int count) { return getSolutions(sudoku, false, count); }
};
