#pragma once

#include <opencv2/opencv.hpp>

class ImageProcessor
{
private:
    cv::Mat image;

public:
    ImageProcessor(const cv::Mat &img) : image(img) {}
    cv::Mat getSudokuImage();
    cv::Mat getCellImage(const cv::Mat &img, int x, int y);
    cv::Mat getProcessedSudokuImage(const cv::Mat &deskewedOriginal);

private:
    cv::Mat cropSquare(cv::Mat thresh);
    cv::Mat makeThresholdedImageForContour(const cv::Mat &img);
    cv::Mat deskewImage(const cv::Mat &cropped);
    void normalizeImageSize();
};
