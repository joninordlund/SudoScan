#include "sudokureader.h"
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <tesseract/baseapi.h>
#include <unistd.h>

std::pair<reader::SudokuGrid, cv::Mat> SudokuReader::getImageData(const cv::Mat& img)
{
    setImage(img);
    cv::Mat processedImg = m_processor->getSudokuImage();
    if (processedImg.empty())
    {
        return std::make_pair(reader::SudokuGrid(), cv::Mat());
    }
    std::pair<reader::SudokuGrid, cv::Mat> data = getSudokuCells(processedImg);
    return data;
}

std::string getExecutablePath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1)
    {
        std::string path(result, count);
        return path.substr(0, path.find_last_of("/"));
    }
    return "";
}

void SudokuReader::setImage(const cv::Mat& img)
{
    m_processor = std::make_unique<ImageProcessor>(img);
    m_tess = std::make_unique<tesseract::TessBaseAPI>();

    // const char *datapath = "/usr/share/tesseract-ocr/5/tessdata/";
    std::string appDir = getExecutablePath();
    std::string tessPath = appDir + "/tessdata/";
    setenv("TESSDATA_PREFIX", tessPath.c_str(), 1);

    if (m_tess->Init(nullptr, "eng", tesseract::OEM_TESSERACT_ONLY))
    {
        std::cerr << "Initializing Tesseract failed\n";
        throw std::runtime_error("Initializing Tesseract failed");
    }

    m_tess->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
    m_tess->SetVariable("debug_file", "/dev/null");
    m_tess->SetVariable("tessedit_char_whitelist", "123456789");
    m_tess->SetVariable("classify_bln_numeric_mode", "1");
    m_tess->SetVariable("enable_new_segsearch", "0");
    m_tess->SetVariable("classify_enable_learning", "0");
    m_tess->SetVariable("classify_enable_adaptive_matcher", "1");
    m_tess->SetVariable("enable_new_segsearch", "0");
}

std::pair <reader::SudokuGrid, cv::Mat> SudokuReader::getSudokuCells(const cv::Mat& processedImg)
{
    // reader::SudokuGrid grid;
    reader::SudokuGrid grid(9, std::vector<int>(9, 0));
    cv::Mat cell;

    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            cell = m_processor->getCellImage(processedImg, x, y);
            cv::bitwise_not(cell, cell);

            m_tess->SetImage(cell.data, cell.cols, cell.rows, cell.channels(), cell.step1());
            m_tess->Recognize(0);

            char* outText = m_tess->GetUTF8Text();
            int confidence = m_tess->MeanTextConf();
            // std::cout << "Number: " << outText << " Conf: " << confidence << std::endl;

            if (outText && outText[0] >= '1' && outText[0] <= '9' && confidence > 71)
            {
                grid[y][x] = outText[0] - '0';
            }
            else
            {
                grid[y][x] = 0;
            }
            delete[] outText;
        }
    }

    cv::Mat fullProcessedImage = m_processor->getProcessedSudokuImage(processedImg);
    // cv::imwrite("debug_final.png", fullProcessedImage);

    return std::make_pair(grid, fullProcessedImage);
}
