#include "sudokuserializer.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

SudokuSerializer::SudokuSerializer()
{
}

bool SudokuSerializer::saveToFile(const QString& fileName, const SudokuBoard& board, const History& history)
{
    QJsonObject root;
    QJsonArray jsonBoard;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            CellData cell = board.data(i, j);
            QJsonObject jsonCell = cellToJson(cell, i, j);
            jsonBoard.append(jsonCell);
        }
    }
    root["board"] = jsonBoard;
    QJsonObject jsonHistory;
    jsonHistory["undo"] = historyStackToJson(history.getUndoStack());
    jsonHistory["redo"] = historyStackToJson(history.getRedoStack());
    root["history"] = jsonHistory;
    QJsonDocument doc(root);

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        return true;
    }
    return false;
}

bool SudokuSerializer::loadFromFile(const QString& fileName, SudokuBoard& board, History& history)
{
    QJsonDocument doc;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QByteArray json = file.readAll();
        doc = QJsonDocument::fromJson(json);
        file.close();
    }
    else
    {
        return false;
    }

    board.clearAll();

    QJsonObject root = doc.object();
    QJsonArray cellArr = root["board"].toArray();
    for (const QJsonValue& cell : cellArr)
    {
        CellData data;
        jsonToCellData(cell, data);
        int row = cell["r"].toInt();
        int col = cell["c"].toInt();
        board.setCellData(row, col, data);
    }

    vector<vector<CellChange>> undoStack;
    QJsonArray undoArr = root["history"].toObject()["undo"].toArray();
    for (const QJsonValue& cmd : undoArr)
    {
        QJsonArray cmdArr = cmd.toArray();
        vector<CellChange> cmdChanges;
        for (const QJsonValue& cmdChange : cmdArr)
        {
            CellChange cellChange;
            jsonToCellChange(cmdChange, cellChange);
            cmdChanges.push_back(cellChange);
        }
        undoStack.push_back(cmdChanges);
    }
    history.setUndoStack((undoStack));

    vector<vector<CellChange>> redoStack;
    QJsonArray redoArr = root["history"].toObject()["redo"].toArray();
    for (const QJsonValue& cmd : redoArr)
    {
        QJsonArray cmdArr = cmd.toArray();
        vector<CellChange> cmdChanges;
        for (const QJsonValue& cmdChange : cmdArr)
        {
            CellChange cellChange;
            jsonToCellChange(cmdChange, cellChange);
            cmdChanges.push_back(cellChange);
        }
        redoStack.push_back(cmdChanges);
    }
    history.setRedoStack(redoStack);
    return true;
}

void SudokuSerializer::jsonToCellChange(const QJsonValue& data, CellChange& cellChange)
{
    cellChange.x = data["r"].toInt();
    cellChange.y = data["c"].toInt();
    QJsonValue old = data["old"];
    QJsonValue neue = data["new"];
    jsonToCellData(old, cellChange.oldState);
    jsonToCellData(neue, cellChange.newState);
}

void SudokuSerializer::jsonToCellData(const QJsonValue& data, CellData& cellData)
{
    cellData.digit = data["digit"].toInt();
    cellData.isGiven = data["given"].toBool();
    cellData.centerMarks = static_cast<uint16_t>(data["center"].toInt());
    cellData.cornerMarks = static_cast<uint16_t>(data["corner"].toInt());
}

QJsonObject SudokuSerializer::cellToJson(const CellData& data, int row, int col)
{
    QJsonObject obj = cellDataToJson(data);
    obj["r"] = row;
    obj["c"] = col;
    return obj;
}

QJsonArray SudokuSerializer::historyStackToJson(const vector<vector<CellChange>>& stack)
{
    QJsonArray jsonStack;
    for (const auto& command : stack)
    {
        QJsonArray jsonCmd;
        for (const CellChange& cellChange : command)
        {
            QJsonObject jsonCellChange;
            jsonCellChange["r"] = cellChange.x;
            jsonCellChange["c"] = cellChange.y;
            jsonCellChange["old"] = cellDataToJson(cellChange.oldState);
            jsonCellChange["new"] = cellDataToJson(cellChange.newState);
            jsonCmd.append(jsonCellChange);
        }
        jsonStack.append(jsonCmd);
    }
    return jsonStack;
}

QJsonObject SudokuSerializer::cellDataToJson(const CellData& data)
{
    QJsonObject obj;
    obj["center"] = data.centerMarks;
    obj["corner"] = data.cornerMarks;
    obj["digit"] = data.digit;
    obj["given"] = data.isGiven;
    return obj;
}
