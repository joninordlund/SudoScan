#include "sudokuimage.h"
#include <QDebug>
#include <opencv2/opencv.hpp>

SudokuImage::SudokuImage(QFrame* parent) :
    QFrame(parent), m_isImgLoaded(false)
{
    m_image = new QLabel(this);
    m_image->setFixedSize(parent->size() - QSize(12, 12));
    m_image->setAlignment(Qt::AlignCenter);
}

void SudokuImage::setIsImgLoaded(bool loaded)
{
    if (m_isImgLoaded != loaded)
    {
        m_isImgLoaded = loaded;
        emit isImgLoadedChanged(m_isImgLoaded);
    }
}

void SudokuImage::load(QString fileName)
{
    cv::Mat origImg = cv::imread(fileName.toStdString());
    if (!origImg.empty())
    {
        if (!setImage(origImg))
        {
            setIsImgLoaded(false);
            qWarning() << "Cannot get sudoku from the image";
            return;
        }
        setIsImgLoaded(true);
    }
}

bool SudokuImage::setImage(cv::Mat origImg)
{
    std::pair<reader::SudokuGrid, cv::Mat> data = m_reader.getImageData(origImg);
    m_origPixmap = cvMatColorToQPixmap(origImg.clone());
    m_origPixmap = m_origPixmap.scaled(
        m_image->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
    m_image->setPixmap(m_origPixmap);

    if (data.second.empty())
    {
        emit imageOk(false);
        return false;
    }

    m_debugPixmap = cvMatGrayscaleToQPixmap(data.second.clone());
    m_debugPixmap = m_debugPixmap.scaled(
        m_image->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
    m_sudokuMatrix = data.first;
    emit imageOk(true);
    return true;
}

void SudokuImage::loadFromData(const QByteArray& data)
{
    std::vector<uchar> buffer(data.begin(), data.end());
    cv::Mat decodedImg = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (!decodedImg.empty())
    {
        setImage(decodedImg);
        setIsImgLoaded(true);
    }
    else
    {
        qWarning() << "decoding pic failed!";
        setIsImgLoaded(false);
    }
}

void SudokuImage::setDebugView(bool visible)
{
    m_image->setPixmap(visible ? m_debugPixmap : m_origPixmap);
}

QPixmap SudokuImage::cvMatGrayscaleToQPixmap(const cv::Mat& mat)
{
    if (mat.empty() || mat.type() != CV_8UC1)
    {
        qWarning() << "Matrix is empty or not grayscale";
        return QPixmap();
    }
    QImage image(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_Grayscale8);
    return QPixmap::fromImage(image.copy());
}

QPixmap SudokuImage::cvMatColorToQPixmap(const cv::Mat& mat)
{
    if (mat.empty() || mat.type() != CV_8UC3)
    {
        qWarning() << "Matrix is empty or not color";
        return QPixmap();
    }
    cv::Mat rgbMat;
    cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
    QImage image(
        rgbMat.data,
        rgbMat.cols,
        rgbMat.rows,
        rgbMat.step,
        QImage::Format_RGB888);
    return QPixmap::fromImage(image.copy());
}

void SudokuImage::onRemove()
{
    setIsImgLoaded(false);
    m_image->clear();

    QPixmap empty(QSize(1, 1));
    empty.fill(Qt::lightGray);
    empty = empty.scaled(
        m_image->size(),
        Qt::KeepAspectRatio);
    m_image->setPixmap(empty);
}

void SudokuImage::onSetImage()
{
    if (isImgLoaded())
    {
        emit newSudoku(m_sudokuMatrix);
    }
}

// void SudokuImage::mousePressEvent(QMouseEvent* event)
// {
//     if (isImgLoaded())
//     {
//         emit newSudoku(m_sudokuMatrix);
//     }
// }
