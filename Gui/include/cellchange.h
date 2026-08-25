#pragma once

#include <cstdint>
using namespace std;

struct CellData {
    int digit = 0;
    bool isGiven = false;
    uint16_t centerMarks = 0;
    uint16_t cornerMarks = 0;

    bool operator==(const CellData& other) const {
        return digit == other.digit &&
               isGiven == other.isGiven &&
               centerMarks == other.centerMarks &&
               cornerMarks == other.cornerMarks;
    }
};

struct CellChange {
    int x;
    int y;
    CellData oldState;
    CellData newState;
};
