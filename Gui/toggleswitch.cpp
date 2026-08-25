#include "toggleswitch.h"
#include <QPainter>
#include <QMouseEvent>

ToggleSwitch::ToggleSwitch(QWidget* parent) :
    QWidget(parent),
    m_checked(false),
    m_position(0),
    m_bgColorUnchecked(QColor(0xcccccc)),
    m_bgColorChecked(QColor(0x4CAF50)),
    m_circleColor(Qt::white),
    m_width(40),  // 54
    m_height(24), // 28
    m_margin(3)
{
    setFixedSize(m_width, m_height);
    setCursor(Qt::PointingHandCursor);

    m_animation = new QPropertyAnimation(this, "position", this);
    m_animation->setDuration(120);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void ToggleSwitch::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;

    int endPos = m_checked ? (m_width - m_height) : 0;
    m_animation->setStartValue(m_position);
    m_animation->setEndValue(endPos);
    m_animation->start();

    emit toggled(m_checked);
}

void ToggleSwitch::setBackgroundColor(const QColor &unchecked, const QColor &checked)
{
    m_bgColorUnchecked = unchecked;
    m_bgColorChecked = checked;
    update();
}

void ToggleSwitch::setCircleColor(const QColor &color)
{
    m_circleColor = color;
    update();
}

void ToggleSwitch::setPosition(int pos)
{
    m_position = pos;
    update();
}

void ToggleSwitch::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Piirretään tausta (pyöristetty suorakulmio)
    QColor bgColor = m_checked ? m_bgColorChecked : m_bgColorUnchecked;
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect(), m_height / 2, m_height / 2);

    // Piirretään liikkuva pallo
    int circleSize = m_height - 2 * m_margin;
    QRect circleRect(m_position + m_margin, m_margin, circleSize, circleSize);

    // Varjo palloon (valinnainen)
    painter.setBrush(QColor(0, 0, 0, 30));
    painter.drawEllipse(circleRect.adjusted(1, 1, 1, 1));

    // Pallo
    painter.setBrush(m_circleColor);
    painter.drawEllipse(circleRect);
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setChecked(!m_checked);
    }
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

QSize ToggleSwitch::sizeHint() const
{
    return QSize(m_width, m_height);
}
