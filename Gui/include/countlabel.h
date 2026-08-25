#pragma once

#include <QLabel>

class CountLabel : public QLabel {

    Q_OBJECT
  public:
    explicit CountLabel(QWidget* parent = nullptr)
        : QLabel(parent) {
        // setAlignment(Qt::AlignCenter);
        setCount(100);
    }

    void setCount(int count);
    int count() const { return m_count; }

  private:
    int m_count = 0;
    void updateColor();
};
