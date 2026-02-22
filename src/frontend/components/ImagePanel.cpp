#include "ImagePanel.h"
#include <QVBoxLayout>
#include <QImage>

ImagePanel::ImagePanel(const QString& title, QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    imageLabel = new QLabel("No Image", this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(400, 400);
    imageLabel->setStyleSheet("background-color: #2b2b2b; color: white;");
    
    layout->addWidget(titleLabel);
    layout->addWidget(imageLabel, 1);
}

void ImagePanel::setImage(const cv::Mat& image) {
    if(image.empty()) return;
    QPixmap pixmap = cvMatToQPixmap(image);
    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImagePanel::clearImage() {
    imageLabel->clear();
    imageLabel->setText("No Image");
}

QPixmap ImagePanel::cvMatToQPixmap(const cv::Mat& inMat) {
    switch (inMat.type()) {
        case CV_8UC3: {
            cv::Mat rgbMat;
            cv::cvtColor(inMat, rgbMat, cv::COLOR_BGR2RGB);
            QImage image((const uchar*)rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
            return QPixmap::fromImage(image.copy());
        }
        case CV_8UC1: {
            QImage image((const uchar*)inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Grayscale8);
            return QPixmap::fromImage(image.copy());
        }
        default:
            return QPixmap();
    }
}