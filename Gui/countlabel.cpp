#include "countlabel.h"

void CountLabel::setCount(int count)
{
    m_count = count;
    setText(QString("C: %1").arg(count));
    updateColor();
}

void CountLabel::updateColor()
{
    QString color;
    QString str("");
    if (m_count == 100)
    {
        color = "darkblue";
        str = "+";
    }
    else if (m_count > 1 && m_count < 100)
    {
        color = "lightblue";
    }
    else if (m_count == 1)
    {
        color = "lightgreen";
    }
    else
    {
        color = "lightcoral";
    }

    setText(QString("C: <span style='color: %1; font-size: 18px;'>%2%3</span>").arg(color).arg(m_count).arg(str));
}
