#pragma once

#include "sudokureader.h"
#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <opencv2/opencv.hpp>

class SudokuImage : public QFrame {
    Q_OBJECT
  public:
    explicit SudokuImage(QFrame* parent = nullptr);

    void load(QString fileName);
    void setDebugView(bool visible);
    void loadFromData(const QByteArray& data);

    bool isImgLoaded() const { return m_isImgLoaded; }
    void setIsImgLoaded(bool loaded);

  private:
    QLabel* m_image;
    SudokuReader m_reader;

    QPixmap cvMatGrayscaleToQPixmap(const cv::Mat& mat);
    QPixmap cvMatColorToQPixmap(const cv::Mat& mat);

    QPixmap m_debugPixmap;
    QPixmap m_origPixmap;

    bool m_isImgLoaded;

    std::vector<std::vector<int>> m_sudokuMatrix;
    bool setImage(cv::Mat img);

  signals:
    void newSudoku(const reader::SudokuGrid& grid);
    void imageOk(const bool success);
    void isImgLoadedChanged(bool loaded);

  public slots:
    void onRemove();
    void onSetImage();

    // protected:
    //   void mousePressEvent(QMouseEvent* event);
};
