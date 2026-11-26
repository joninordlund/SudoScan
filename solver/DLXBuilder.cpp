#include "DLXBuilder.h"
#include <algorithm>
#include <cassert>
#include <limits>
#include <random>

vector<vector<int>> DLXBuilder::findSolutions(int maxCount)
{
    m_maxCount = maxCount;

    // auto start = std::chrono::high_resolution_clock::now();
    build();
    search(0);
    // auto end = std::chrono::high_resolution_clock::now();

    // auto durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // double durationMs = durationUs / 1000.0;

    // std::cout << "[PERF] getRandomSudokuSet: " << durationMs << " ms (" << durationUs
    //           << " µs) | Sols: " << m_allSolutions.size() << endl;

    return m_allSolutions;
}

void DLXBuilder::build()
{
    const int R = (int)m_rows.size();

    // 0 = root, 1..N = columns, spacer, (4 nodes, spacer) * R = rows
    m_nodes.assign(N + 2 + 5 * R, Node());

    m_nodes[0].left = N;
    m_nodes[0].right = 1;
    m_nodes[0].top = 0;
    m_nodes[0].row = 0;
    m_nodes[0].len = -1;

    for (int c = 1; c <= N; c++)
    {
        Node& h = m_nodes[c];
        h.left = c - 1;
        h.right = (c == N) ? 0 : c + 1;
        h.up = h.down = c;
        h.top = c;
        h.row = 0;
        h.len = 0;
    }

    vector<int> last(N + 1);
    for (int c = 0; c <= N; c++)
        last[c] = c;

    // ensimmäinen spacer (top = -1)
    int idx = N + 1;
    m_nodes[idx].top = -1;

    for (int r = 0; r < R; r++)
    {
        int base = N + 2 + r * 5;
        for (int k = 0; k < 4; k++)
        {
            int id = base + k;
            int c = m_rows[r][k] + 1;
            Node& nd = m_nodes[id];
            nd.top = c;
            nd.row = r + 1;
            nd.up = last[c];
            nd.down = c;
            m_nodes[last[c]].down = id;
            m_nodes[c].up = id;
            last[c] = id;
            m_nodes[c].len++;
        }
        m_nodes[base - 1].down = base + 3;
        Node& sp = m_nodes[base + 4];
        sp.top = -1;
        sp.up = base;
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

    int col = chooseColMRV();
    cover(col);

    vector<int> rowCandidates;
    int r = m_nodes[col].down;
    while (r != col)
    {
        rowCandidates.push_back(r);
        r = m_nodes[r].down;
    }
    if (m_randomMode)
    {
        std::shuffle(rowCandidates.begin(), rowCandidates.end(), m_rndGen);
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
