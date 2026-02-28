#include "ImagePanel.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QImage>

ImagePanel::ImagePanel(const QString& title, bool isInput, QWidget *parent) 
    : QWidget(parent), isInput(isInput) 
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(8);
    
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("font-weight: bold; color: #2d3436; font-size: 14px;");
    layout->addWidget(titleLabel);

    imageDisplay = new QLabel("Drop Image Here", this);
    imageDisplay->setObjectName("imageDisplay");
    imageDisplay->setAlignment(Qt::AlignCenter);
    imageDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(imageDisplay);

    if (isInput) {
        QPushButton* loadBtn = new QPushButton("Load Image", this);
        loadBtn->setCursor(Qt::PointingHandCursor);
        connect(loadBtn, &QPushButton::clicked, this, &ImagePanel::handleLoadImage);
        layout->addWidget(loadBtn);
    }
}

cv::Mat ImagePanel::getImage() const { return currentImage; }

void ImagePanel::displayImage(const cv::Mat& img) {
    if (img.empty()) return;
    currentImage = img.clone();
    currentPixmap = cvMatToPixmap(currentImage);
    
    // Remove the dashed border to make it look clean once loaded
    imageDisplay->setStyleSheet("background-color: transparent; border: none;"); 
    updateDisplay();
}

void ImagePanel::updateDisplay() {
    // Dynamically rescales the original high-res pixmap to whatever size the panel is right now
    if (!currentPixmap.isNull() && imageDisplay->size().width() > 10) {
        imageDisplay->setPixmap(currentPixmap.scaled(imageDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ImagePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateDisplay(); // Redraw perfectly on full-screen!
}

void ImagePanel::clear() {
    currentImage = cv::Mat();
    currentPixmap = QPixmap();
    imageDisplay->clear();
    imageDisplay->setText("No Image Loaded");
    // Restore the styling
    imageDisplay->setStyleSheet("background-color: rgba(255, 255, 255, 0.85); border: 2px dashed #B2BEC3; border-radius: 8px; color: #636E72;");
}

void ImagePanel::handleLoadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat img = cv::imread(fileName.toStdString());
        if(!img.empty()) displayImage(img);
    }
}

QPixmap ImagePanel::cvMatToPixmap(const cv::Mat& img) {
    if (img.empty()) return QPixmap();
    cv::Mat temp;
    if (img.channels() == 3) {
        cv::cvtColor(img, temp, cv::COLOR_BGR2RGB);
        QImage qimg((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        return QPixmap::fromImage(qimg.copy());
    } else {
        QImage qimg((const uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
        return QPixmap::fromImage(qimg.copy());
    }
}