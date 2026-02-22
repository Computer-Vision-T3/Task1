#include "ImagePanel.h"
#include <QVBoxLayout>
#include <QImage>
#include <QFileDialog>

ImagePanel::ImagePanel(const QString& title, bool isInput, QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(titleLabel);

    // If this is an input panel, give it its own upload button
    if (isInput) {
        uploadBtn = new QPushButton("Load Image", this);
        layout->addWidget(uploadBtn);
        connect(uploadBtn, &QPushButton::clicked, this, &ImagePanel::handleUpload);
    }

    imageLabel = new QLabel("No Image", this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(250, 250);
    imageLabel->setStyleSheet("background-color: #2b2b2b; color: white; border: 1px solid #555;");
    
    layout->addWidget(imageLabel, 1);
}

void ImagePanel::handleUpload() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat image = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
        if(!image.empty()) {
            setImage(image);
            emit imageUploaded();
        }
    }
}

void ImagePanel::setImage(const cv::Mat& image) {
    if(image.empty()) return;
    currentImage = image.clone();
    QPixmap pixmap = cvMatToQPixmap(image);
    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImagePanel::clearImage() {
    currentImage.release();
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
        default: return QPixmap();
    }
}