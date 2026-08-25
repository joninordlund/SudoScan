#pragma once
#include <vector>

#include "sudokusolver.h"

using namespace std;

enum class Symmetry {
    None,
    Rotate180,
    Vertical,
    Horizontal
};

class SolutionSet
{
  public:
    SolutionSet()
        : m_index(0), m_maxSolCount(25), m_solutions{{}} {}

    void updateSolutions(const Matrix& matrix, int maxCount);
    vector<Matrix> getRandomSudokuSet(const Matrix& matrix, int count);
    int currentIndex() const { return m_index; }
    Matrix next();
    Matrix prev();

    int count() const { return m_solutions.size(); }
    int maxCount() const { return m_maxSolCount; }

    Matrix generate(int target, Symmetry symmetry);

  private:
    SudokuSolver m_solver;
    vector<Matrix> m_solutions;
    int m_index;
    int m_maxSolCount;
    bool isUnique(const Matrix& matrix);
};
