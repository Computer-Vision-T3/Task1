#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <opencv2/opencv.hpp>

class ImagePanel : public QWidget {
    Q_OBJECT

public:
    explicit ImagePanel(const QString& title, bool isInput = false, QWidget *parent = nullptr);
    void setImage(const cv::Mat& image);
    void clearImage();
    cv::Mat getCurrentImage() const { return currentImage; } // For the backend to grab

signals:
    void imageUploaded(); // Tells the controller state has changed

private slots:
    void handleUpload();

private:
    QLabel* titleLabel;
    QLabel* imageLabel;
    QPushButton* uploadBtn;
    cv::Mat currentImage;
    QPixmap cvMatToQPixmap(const cv::Mat& inMat);
};

#endif // IMAGEPANEL_H