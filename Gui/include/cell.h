#pragma once
#include <QWidget>

class Cell : public QWidget
{
    Q_OBJECT
  public:
    Cell(int row, int col, QWidget* parent = nullptr);
    void setDigit(int digit, bool isGiven);
    void setSelected(bool sel);
    void setCursor(bool cursor);
    void setGiven(bool given);

    bool isCursor() const { return m_isCursor; }
    bool isGiven() const { return m_isGiven; }
    int row() const { return m_row; }
    int col() const { return m_col; }
    int digit() const { return m_digit; }
    void setCornerMarks(uint16_t marks) { m_cornerMarks = marks; }
    void setCenterMarks(uint16_t marks) { m_centerMarks = marks; }
    void clearCornerMarks() { m_cornerMarks = 0; }
    void clearCenterMarks() { m_centerMarks = 0; }

    void drawCenterMarks(QPainter& painter, const QRect& targetRect);
    void drawCornerMarks(QPainter& painter, const QRect& targetRect);

  protected:
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

  private:
    int m_digit;
    uint16_t m_cornerMarks;
    uint16_t m_centerMarks;
    int m_row;
    int m_col;
    bool m_isGiven;
    bool m_isLocked;
    bool m_isSelected;
    bool m_isCursor;

signals:
    void cellClicked(Cell *cell, bool ctrl);
};
