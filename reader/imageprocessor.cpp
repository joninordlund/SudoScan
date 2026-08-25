#include "imageprocessor.h"
#include <algorithm>
#include <iostream>

cv::Mat ImageProcessor::getSudokuImage()
{
    normalizeImageSize();
    cv::Mat thresh = makeThresholdedImageForContour(this->image);
    cv::Mat cropped = cropSquare(thresh);
    if (cropped.empty())
    {
        return cv::Mat();
    }
    cv::Mat temp = deskewImage(cropped);
    if (temp.empty())
    {
        return cv::Mat();
    }
    cv::Mat deskewedSudoku;
    cv::bitwise_not(temp, deskewedSudoku);
    return deskewedSudoku;
}

void ImageProcessor::normalizeImageSize()
{
    int targetWidth = 1200;
    if (this->image.cols > targetWidth)
    {
        double scale = static_cast<double>(targetWidth) / this->image.cols;
        cv::resize(this->image, this->image, cv::Size(), scale, scale);
    }
}

cv::Mat ImageProcessor::makeThresholdedImageForContour(const cv::Mat& img)
{
    cv::Mat gray, thresh, blurred, otsu;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    cv::adaptiveThreshold(blurred, thresh, 255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, 25, 8);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(thresh, thresh, cv::MORPH_CLOSE, kernel);

    cv::imwrite("/home/linuxguy/tmp/debug_failed_thresh.png", thresh);
    return thresh;
}

cv::Mat ImageProcessor::cropSquare(cv::Mat thresh)
{
    cv::Mat dilated;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(thresh, dilated, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double maxArea = 0;
    std::vector<cv::Point> bestApprox;
    double totalImageArea = static_cast<double>(image.cols * image.rows);

    for (const auto& cnt : contours)
    {
        double area = cv::contourArea(cnt);
        if (area < totalImageArea * 0.1)
        {
            continue;
        }

        std::vector<cv::Point> hull;
        cv::convexHull(cnt, hull);

        double epsilon = 0.02 * cv::arcLength(hull, true);
        std::vector<cv::Point> approx;
        cv::approxPolyDP(hull, approx, epsilon, true);

        if (approx.size() == 4)
        {
            if (area > maxArea)
            {
                maxArea = area;
                bestApprox = approx;
            }
        }
    }

    if (!bestApprox.empty())
    {
        cv::Rect box = cv::boundingRect(bestApprox);
        box &= cv::Rect(0, 0, image.cols, image.rows);
        return image(box).clone();
    }

    std::cerr << "Error: couldn't find grid (even by using convex hull)" << std::endl;
    return cv::Mat();
}

cv::Mat ImageProcessor::deskewImage(const cv::Mat& cropped)
{
    cv::Mat thresh;
    thresh = makeThresholdedImageForContour(cropped);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> bestContour;
    double maxArea = 0.0;

    // find the largest square in the image
    for (const auto& curve : contours)
    {
        double area = cv::contourArea(curve);
        if (area > maxArea)
        {
            double epsilon = 0.02 * cv::arcLength(curve, true);
            std::vector<cv::Point> approx;
            cv::approxPolyDP(curve, approx, epsilon, true);

            if (approx.size() == 4 && area > 1000)
            {
                maxArea = area;
                bestContour = approx;
            }
        }
    }
    if (bestContour.size() != 4)
    {
        cv::imwrite("/home/linuxguy/tmp/debug_failed_best.png", thresh);
        std::cerr << "deskew: Error: couldn't find a suitable square" << std::endl;
        return cv::Mat();
    }

    // For the cv::getPerspectiveTransform function, points of the square
    // must be in the following order: top-left,
    // top-right, bottom-right and bottom-left. Also cv::Point2f-type is needed.

    std::vector<cv::Point2f> pts(4);
    std::vector<cv::Point2f> ordPts(4);
    std::vector<std::pair<double, int>> sums(4);
    std::vector<std::pair<double, int>> diffs(4);

    for (int i = 0; i < 4; i++)
    {
        double bestCntX = (float)bestContour[i].x;
        double bestCntY = (float)bestContour[i].y;

        pts[i] = cv::Point2f(bestCntX, bestCntY);
        sums[i] = std::make_pair(bestCntX + bestCntY, i);
        diffs[i] = std::make_pair(bestCntX - bestCntY, i);
    }

    std::sort(sums.begin(), sums.end());
    std::sort(diffs.begin(), diffs.end());

    ordPts[0] = pts[sums[0].second];  // top-left (the smallest sum)
    ordPts[1] = pts[diffs[3].second]; // top-right (the largest difference)
    ordPts[2] = pts[sums[3].second];  // bottom-right (the largest sum)
    ordPts[3] = pts[diffs[0].second]; // bottom-left (the smallest difference)

    int width = 900;
    int height = 900;
    std::vector<cv::Point2f> destPts(4);

    destPts[0] = cv::Point2f(0, 0);
    destPts[1] = cv::Point2f((float)(width - 1), 0);
    destPts[2] = cv::Point2f((float)(width - 1), (float)(height - 1));
    destPts[3] = cv::Point2f(0, (float)(height - 1));

    cv::Mat perspectiveTr = cv::getPerspectiveTransform(ordPts, destPts);

    cv::Mat deskewedImage, gray;
    cv::cvtColor(cropped, gray, cv::COLOR_BGR2GRAY);
    cv::warpPerspective(thresh, deskewedImage, perspectiveTr, cv::Size(width, height));
    cv::imwrite("/home/linuxguy/tmp/debug_failed_contours2.png", deskewedImage);
    return deskewedImage;
}

cv::Mat ImageProcessor::getCellImage(const cv::Mat& img, int x, int y)
{
    int singleCellSize = img.rows / 9;
    int y_start_correction = 0;
    int margin = 9;
    cv::Mat cell = img(cv::Rect(
                           x * singleCellSize + margin,
                           y * singleCellSize + margin + y_start_correction,
                           singleCellSize - 2 * margin,
                           singleCellSize - 2 * margin))
                       .clone();
    cv::Mat thresh;
    cv::bitwise_not(cell, thresh);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Rect bestBBox;
    double maxArea = 0.0;

    for (const auto& cnt : contours)
    {
        double area = cv::contourArea(cnt);
        if (area > maxArea && area > (thresh.cols * thresh.rows) * 0.05)
        {
            maxArea = area;
            bestBBox = cv::boundingRect(cnt);
        }
    }

    if (bestBBox.area() > 0)
    {
        int margin = 3;
        cv::Rect finalRoi = bestBBox;
        finalRoi.x = std::max(0, finalRoi.x - margin);
        finalRoi.y = std::max(0, finalRoi.y - margin);

        finalRoi.width = std::min(thresh.cols - finalRoi.x, finalRoi.width + 2 * margin);
        finalRoi.height = std::min(thresh.rows - finalRoi.y, finalRoi.height + 2 * margin);

        cv::Mat croppedNumber = thresh(finalRoi).clone();

        int padding = 10;
        cv::copyMakeBorder(croppedNumber, croppedNumber,
                           padding,
                           padding,
                           padding,
                           padding,
                           cv::BORDER_CONSTANT,
                           cv::Scalar(0));
        int DESIRED_SIZE = 100;

        cv::Mat resizedNumber;
        double aspectRatio = (double)croppedNumber.cols / croppedNumber.rows;
        int newWidth, newHeight;

        if (aspectRatio > 1.0)
        {
            newWidth = DESIRED_SIZE;
            newHeight = cvRound(DESIRED_SIZE / aspectRatio);
        }
        else
        {
            newHeight = DESIRED_SIZE;
            newWidth = cvRound(DESIRED_SIZE * aspectRatio);
        }

        cv::resize(croppedNumber, resizedNumber, cv::Size(newWidth, newHeight));
        cv::Mat finalCell = cv::Mat::zeros(DESIRED_SIZE, DESIRED_SIZE, resizedNumber.type());

        int x_offset = (DESIRED_SIZE - newWidth) / 2;
        int y_offset = (DESIRED_SIZE - newHeight) / 2;

        resizedNumber.copyTo(finalCell(cv::Rect(x_offset, y_offset, newWidth, newHeight)));
        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(finalCell, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours)
        {
            double area = cv::contourArea(contour);
            if (area < 100)
            {
                cv::drawContours(finalCell, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(0), -1);
            }
        }
        return finalCell;
    }
    else
    {
        return cv::Mat::zeros(100, 100, CV_8UC1);
    }
}

cv::Mat ImageProcessor::getProcessedSudokuImage(const cv::Mat& deskewedOriginal)
{
    int singleCellSize = deskewedOriginal.rows / 9;

    cv::Mat finalImage;
    for (int y = 0; y < 9; y++)
    {
        cv::Mat rowImage;
        for (int x = 0; x < 9; x++)
        {

            cv::Mat cell = getCellImage(deskewedOriginal, x, y);

            cv::Mat resizedCell;
            cv::bitwise_not(cell, cell);
            cv::resize(cell, resizedCell, cv::Size(singleCellSize, singleCellSize));

            if (x == 0)
            {
                rowImage = resizedCell;
            }
            else
            {
                cv::hconcat(rowImage, resizedCell, rowImage);
            }
        }

        if (y == 0)
        {
            finalImage = rowImage;
        }
        else
        {
            cv::vconcat(finalImage, rowImage, finalImage);
        }
    }
    cv::imwrite("/home/linuxguy/tmp/debug_failed_final.png", finalImage);
    return finalImage;
}
