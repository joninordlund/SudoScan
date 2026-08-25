#pragma once

#include "sudokusolver.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
using Matrix = vector<vector<int>>;

struct Node
{
    void print(int idx, int N)
    {
        int x = top;
        string str = " ,top ";
        if (idx <= N)
        {
            x = len;
            str = " ,len ";
        }
        cout << "idx: " << idx << ", row " << row << ", up " << up << ", down " << down << ", left " << left << ", right " << right << str << x << endl;
    }
    int up, down, left, right, top, len, row;
    Node() : up(-1), down(-1), left(-1), right(-1), top(-1), len(-1), row(-1) {}
};

class DLXBuilder
{
public:
  DLXBuilder(Matrix mat, int maxSolCount = 64)
      : m_matrix(mat), m_maxCount(maxSolCount), m_rndGen(std::random_device{}()) {
      M = m_matrix.size();
      N = m_matrix[0].size();
      m_solution.resize(M);
  };
    vector<vector<int>> findSolutions(int maxCount);
    void setRandomMode(bool enabled) { m_randomMode = enabled; }

private:
    vector<Node> m_nodes;
    Matrix m_matrix;
    vector<int> lenVec;
    vector<int> m_solution;
    int N, M;
    vector<vector<int>> m_allSolutions;
    int m_maxCount;
    bool m_randomMode;
    std::mt19937 m_rndGen;

    void build();
    void search(int x);
    void cover(int col);
    void uncover(int col);
    void hide(int i);
    void unhide(int i);
    int chooseCol();
    int chooseColRandom();
    int chooseColMRV();
};
