#pragma once

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include "imageprocessor.h"

namespace reader {
using SudokuGrid = std::vector<std::vector<int>>;
}

class SudokuReader
{
private:
  cv::Mat m_image;
  std::unique_ptr<tesseract::TessBaseAPI> m_tess;
  std::unique_ptr<ImageProcessor> m_processor;

  void setImage(const cv::Mat& img);
  std::pair<reader::SudokuGrid, cv::Mat> getSudokuCells(const cv::Mat& processedImg);

public:
    SudokuReader(){};
    std::pair<reader::SudokuGrid, cv::Mat> getImageData(const cv::Mat& img);
    const ImageProcessor& getProcessor() const { return *m_processor; }
};
