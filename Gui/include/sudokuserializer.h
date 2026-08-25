#pragma once

#include "history.h"
#include "sudokuboard.h"

// #include <QJsonObject>
#include <QString>

class SudokuSerializer {
  public:
    SudokuSerializer();

    static bool saveToFile(const QString& fileName,
                           const SudokuBoard& board,
                           const History& history);

    static bool loadFromFile(const QString& fileName,
                             SudokuBoard& board,
                             History& history);

  private:
    static QJsonObject cellToJson(const CellData& data, int row, int col);
    static QJsonArray historyStackToJson(const vector<vector<CellChange>>& stack);
    static QJsonObject cellDataToJson(const CellData& state);
    static void jsonToCellData(const QJsonValue& data, CellData& cellData);
    static void jsonToCellChange(const QJsonValue& data, CellChange& cellChange);
};
