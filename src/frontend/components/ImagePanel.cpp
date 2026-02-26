#include "ImagePanel.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QImage>

ImagePanel::ImagePanel(const QString& title, bool isInput, QWidget *parent) 
    : QWidget(parent), isInput(isInput) 
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(5);
    
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);

    imageDisplay = new QLabel("Drop Image Here", this);
    imageDisplay->setAlignment(Qt::AlignCenter);
    imageDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageDisplay->setStyleSheet(
        "background-color: #f0f0f0; "
        "border: 2px dashed #ccc; "
        "border-radius: 5px; "
        "color: #888;"
    );
    layout->addWidget(imageDisplay);

    if (isInput) {
        QPushButton* loadBtn = new QPushButton("Load Image", this);
        loadBtn->setCursor(Qt::PointingHandCursor);
        loadBtn->setStyleSheet("padding: 5px; background-color: #e1e1e1;");
        connect(loadBtn, &QPushButton::clicked, this, &ImagePanel::handleLoadImage);
        layout->addWidget(loadBtn);
    }
}

// Definition for getImage (Fixes AppController error)
cv::Mat ImagePanel::getImage() const {
    return currentImage;
}

// Definition for displayImage
void ImagePanel::displayImage(const cv::Mat& img) {
    if (img.empty()) return;
    currentImage = img.clone();
    
    QPixmap pix = cvMatToPixmap(currentImage);
    imageDisplay->setPixmap(pix.scaled(imageDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageDisplay->setStyleSheet("background-color: #222; border: 1px solid #444;"); 
}

// Definition for clear (Fixes AppController error)
void ImagePanel::clear() {
    currentImage = cv::Mat();
    imageDisplay->clear();
    imageDisplay->setText("No Image Loaded");
    imageDisplay->setStyleSheet("background-color: #f0f0f0; border: 2px dashed #ccc;");
}

// Definition for handleLoadImage (Fixes ImagePanel error)
void ImagePanel::handleLoadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat img = cv::imread(fileName.toStdString());
        if(!img.empty()) {
            displayImage(img);
        }
    }
}

// Definition for cvMatToPixmap (Fixes ImagePanel error)
QPixmap ImagePanel::cvMatToPixmap(const cv::Mat& img) {
    if (img.empty()) return QPixmap();

    cv::Mat temp;
    if (img.channels() == 3) {
        // Convert BGR (OpenCV) to RGB (Qt)
        cv::cvtColor(img, temp, cv::COLOR_BGR2RGB);
        QImage qimg((const uchar*)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        return QPixmap::fromImage(qimg.copy());
    } else {
        // Grayscale
        QImage qimg((const uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
        return QPixmap::fromImage(qimg.copy());
    }
}
