#include "DLXBuilder.h"
#include <cassert>
#include <chrono>
#include <limits>

vector<vector<int>> DLXBuilder::findSolutions(int maxCount)
{
    m_maxCount = maxCount;
    build();
    auto start = std::chrono::high_resolution_clock::now();
    search(0);
    auto end = std::chrono::high_resolution_clock::now();

    auto durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double durationMs = durationUs / 1000.0;

    std::cout << "[PERF] getRandomSudokuSet: " << durationMs << " ms (" << durationUs
              << " µs) | Sols: " << m_allSolutions.size() << endl;

    return m_allSolutions;
}

void DLXBuilder::build()
{
    int M = m_matrix.size();
    int N = m_matrix[0].size();
    m_lenVec.resize(N);

    // To simplify the handling, replace 1's in the matrix by
    // node indexes
    int idx = N + 2;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (m_matrix[i][j] > 0)
            {
                m_matrix[i][j] = idx++;
                m_lenVec[j]++;
            }
        }
        // skip spacer indexes
        idx++;
    }
    // attach the header
    vector<int> head;
    for (int i = 1; i <= N; i++)
    {
        head.push_back(i);
    }
    m_matrix.insert(m_matrix.begin(), head);

    int maxSize = N * M + N + (N + 1); // matrix + header + spacers
    m_nodes.clear();
    m_nodes.resize(maxSize);

    // set uo the root node
    m_nodes[0].up = -1;
    m_nodes[0].down = -1;
    m_nodes[0].left = N;
    m_nodes[0].right = 1;
    m_nodes[0].top = 0;
    m_nodes[0].row = 0;
    m_nodes[0].len = -1;

    auto posMod = [](int a, int b)
    {
        return ((a % b) + b) % b;
    };

    // link the columns
    int nodeCount = 0;
    for (int i = 0; i < M + 1; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (m_matrix[i][j] > 0)
            {
                int f = posMod(i - 1, M + 1);
                while (true)
                {
                    if (m_matrix[f][j] > 0)
                        break;
                    f = posMod(f - 1, M + 1);
                }
                int up = m_matrix[f][j];
                f = posMod(i + 1, M + 1);
                while (true)
                {
                    if (m_matrix[f][j] > 0)
                        break;
                    f = posMod(f + 1, M + 1);
                }
                int down = m_matrix[f][j];
                Node node;
                node.up = up;
                node.down = down;
                node.left = -1;
                node.right = -1;
                node.row = i;
                node.top = j + 1;
                if (i == 0)
                {
                    node.left = posMod(j, N + 1);
                    node.right = posMod(j + 2, N + 1);
                    node.len = m_lenVec[j];
                }
                else
                {
                    node.len = -1;
                }
                m_nodes[m_matrix[i][j]] = node;
                nodeCount++;
            }
        }
    }
    m_nodes.resize(nodeCount + M + 2);
    // set the spacers
    int prev = -1;
    for (int i = N + 1; i < m_nodes.size(); i++)
    {
        if (m_nodes[i].top < 0)
        {
            if (prev >= 0)
            {
                m_nodes[i].up = prev + 1;
                m_nodes[prev].down = i - 1;
            }
            prev = i;
        }
    }
}

void DLXBuilder::cover(int col)
{
    int left = m_nodes[col].left;
    int right = m_nodes[col].right;
    m_nodes[right].left = left;
    m_nodes[left].right = right;

    int p = m_nodes[col].down;
    while (p != col)
    {
        hide(p);
        p = m_nodes[p].down;
    }
}

void DLXBuilder::hide(int row)
{
    int q = row + 1;
    while (q != row)
    {
        int x = m_nodes[q].top;
        if (x < 0) // q is a spacer
        {
            q = m_nodes[q].up;
            continue;
        }
        int up = m_nodes[q].up;
        int down = m_nodes[q].down;

        m_nodes[up].down = down;
        m_nodes[down].up = up;
        m_nodes[x].len = m_nodes[x].len - 1;
        q++;
    }
}

void DLXBuilder::uncover(int col)
{
    int p = m_nodes[col].up;
    while (p != col)
    {
        unhide(p);
        p = m_nodes[p].up;
    }
    int left = m_nodes[col].left;
    int right = m_nodes[col].right;
    m_nodes[left].right = col;
    m_nodes[right].left = col;
}

void DLXBuilder::unhide(int row)
{
    int q = row - 1;
    while (q != row)
    {
        int x = m_nodes[q].top;
        if (x < 0) // q is a spacer
        {
            q = m_nodes[q].down;
            continue;
        }
        int up = m_nodes[q].up;
        int down = m_nodes[q].down;

        m_nodes[up].down = q;
        m_nodes[down].up = q;
        m_nodes[x].len = m_nodes[x].len + 1;
        q--;
    }
}

void DLXBuilder::search(int x)
{
    if (m_nodes[0].right == 0)
    {
        vector<int> currentSolution(m_solution.begin(), m_solution.begin() + x);
        m_allSolutions.push_back(currentSolution);

        return;
    }

    int col = chooseCol();
    cover(col);

    vector<int> rowCandidates;
    int r = m_nodes[col].down;
    while (r != col)
    {
        rowCandidates.push_back(r);
        r = m_nodes[r].down;
    }

    for (int r : rowCandidates)
    {
        m_solution[x] = m_nodes[r].row;

        int j = r + 1;
        while (j != r)
        {
            if (m_nodes[j].top < 0) // spacer
            {
                j = m_nodes[j].up;
                continue;
            }
            cover(m_nodes[j].top);
            j++;
        }

        search(x + 1);

        int t = r - 1;
        while (t != r)
        {
            if (m_nodes[t].top < 0) // spacer
            {
                t = m_nodes[t].down;
                continue;
            }
            uncover(m_nodes[t].top);
            t--;
        }

        if (m_allSolutions.size() >= m_maxCount)
        {
            break;
        }
    }
    uncover(col);
}

int DLXBuilder::chooseCol()
{
    if (m_randomMode)
    {
        return chooseColRandom();
    }
    return chooseColMRV();
}

// choose the first column with minimum length (MRV heuristic)
int DLXBuilder::chooseColMRV()
{
    int best = -1;
    int bestLen = numeric_limits<int>::max();

    for (int col = m_nodes[0].right; col != 0; col = m_nodes[col].right)
    {
        if (m_nodes[col].len < bestLen)
        {
            bestLen = m_nodes[col].len;
            best = col;
        }
    }
    return best;
}

int DLXBuilder::chooseColRandom()
{
    int minSize = numeric_limits<int>::max();
    int bestCol = m_nodes[0].right;
    int count = 0;

    for (int col = m_nodes[0].right; col != 0; col = m_nodes[col].right)
    {
        int len = m_nodes[col].len;
        if (len < minSize)
        {
            minSize = len;
            bestCol = col;
            count = 1;
        }
        else if (len == minSize)
        {
            count++;
            std::uniform_int_distribution<int> dist(1, count);
            if (dist(m_rndGen) == 1)
            {
                bestCol = col;
            }
        }
    }
    return bestCol;
}
