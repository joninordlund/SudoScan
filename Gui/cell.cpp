#include "cell.h"
#include <QPainter>
#include <QMouseEvent>

Cell::Cell(int row, int col, QWidget* parent) :
    QWidget(parent),
    m_row(row),
    m_col(col),
    m_isSelected(false),
    m_isCursor(false),
    m_centerMarks(0),
    m_cornerMarks(0)
{
    setFixedSize(70, 70);
    setFocusPolicy(Qt::StrongFocus);
}

void Cell::setSelected(bool sel)
{
    m_isSelected = sel;
    update();
}

void Cell::setDigit(int digit, bool isGiven)
{
    m_digit = digit;
    m_isGiven = isGiven;
    update();
}

void Cell::setCursor(bool cursor)
{
    m_isCursor = cursor;
    update();
}

void Cell::setGiven(bool given)
{
    m_isGiven = given;
}

void Cell::mousePressEvent(QMouseEvent *event)
{
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    emit cellClicked(this, ctrl);
}
void Cell::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isSelected)
    {
        painter.fillRect(rect(), QColor(220, 230, 255));
    }
    else
    {
        painter.fillRect(rect(), Qt::white);
    }

    painter.setPen(QPen(Qt::lightGray, 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));

    painter.setPen(Qt::NoPen);
    int thick = 3;

    if (m_col % 3 == 0)
    {
        painter.fillRect(0, 0, thick, height(), Qt::black);
    }

    if (m_col == 8)
    {
        painter.fillRect(width() - thick, 0, thick, height(), Qt::black);
    }

    if (m_row % 3 == 0)
    {
        painter.fillRect(0, 0, width(), thick, Qt::black);
    }
    if (m_row == 8)
    {
        painter.fillRect(0, height() - thick, width(), thick, Qt::black);
    }

    QRect textRect = rect().adjusted(thick, thick, -thick, -thick);

    if (m_digit != 0)
    {
        painter.setFont(QFont("Arial", 26, m_isGiven ? QFont::Bold : QFont::Normal));
        painter.setPen(m_isGiven ? Qt::black : QColor(0, 102, 204));
        painter.drawText(textRect, Qt::AlignCenter, QString::number(m_digit));
    }
    else
    {
        drawCenterMarks(painter, textRect);
        drawCornerMarks(painter, textRect);
    }
}

void Cell::drawCenterMarks(QPainter& painter, const QRect& targetRect)
{
    if (m_digit != 0)
    {
        return;
    }
    QString text;
    int count = 0;
    for (int i = 1; i <= 9; ++i)
    {
        if (m_centerMarks & (1 << (i - 1)))
        {
            text += QString::number(i);
            count++;
        }
    }

    if (count == 0)
    {
        return;
    }

    int fontSize = height() / 5;
    if (count > 6)
    {
        fontSize = height() / 6;
    }
    if (count > 7)
    {
        fontSize = height() / 7;
    }

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", fontSize, QFont::Normal, true));
    painter.drawText(targetRect, Qt::AlignCenter, text);
}

void Cell::drawCornerMarks(QPainter& painter, const QRect& targetRect)
{
    if (m_digit != 0)
    {
        return;
    }

    painter.setPen(Qt::black);
    QVector<int> activeMarks;
    for (int i = 1; i <= 9; ++i)
    {
        if (m_cornerMarks & (1 << (i - 1)))
        {
            activeMarks.append(i);
        }
    }

    if (activeMarks.isEmpty())
    {
        return;
    }

    static const QPointF positions[] = {
        {0, 0}, {2, 0}, {0, 2}, {2, 2}, {1, 0}, {1, 2}, {0, 1}, {2, 1}, {1, 1}};
    int w = targetRect.width() / 3;
    int h = targetRect.height() / 3;
    painter.setFont(QFont("Arial", h * 0.8));

    for (int i = 0; i < activeMarks.size() && i < 9; ++i)
    {
        int posX = targetRect.x() + (positions[i].x() * w);
        int posY = targetRect.y() + (positions[i].y() * h);

        QRect subRect(posX, posY, w, h);
        painter.drawText(subRect, Qt::AlignCenter, QString::number(activeMarks[i]));
    }
}
