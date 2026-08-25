#pragma once

#include <QWidget>
#include <QPropertyAnimation>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int position READ position WRITE setPosition)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);

    bool isChecked() const { return m_checked; }
    void setChecked(bool checked);

    void setBackgroundColor(const QColor &unchecked, const QColor &checked);
    void setCircleColor(const QColor &color);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;

private:
    int position() const { return m_position; }
    void setPosition(int pos);

    bool m_checked;
    int m_position;
    QPropertyAnimation *m_animation;

    QColor m_bgColorUnchecked;
    QColor m_bgColorChecked;
    QColor m_circleColor;

    int m_width;
    int m_height;
    int m_margin;
};
