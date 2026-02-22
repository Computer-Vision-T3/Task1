#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <QWidget>
#include <QLabel>
#include <opencv2/opencv.hpp>

class ImagePanel : public QWidget {
    Q_OBJECT

public:
    explicit ImagePanel(const QString& title, QWidget *parent = nullptr);
    void setImage(const cv::Mat& image);
    void clearImage();

private:
    QLabel* titleLabel;
    QLabel* imageLabel;
    QPixmap cvMatToQPixmap(const cv::Mat& inMat);
};

#endif // IMAGEPANEL_H