#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QResizeEvent>
#include <opencv2/opencv.hpp>

class ImagePanel : public QWidget {
    Q_OBJECT
public:
    explicit ImagePanel(const QString& title, bool isInput, QWidget *parent = nullptr);

    cv::Mat getImage() const;
    void displayImage(const cv::Mat& img);
    void clear();

protected:
    // This captures window maximize/resize to fix the image scaling bug
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void handleLoadImage();

private:
    QLabel* imageDisplay;
    cv::Mat currentImage; 
    QPixmap currentPixmap; // Stores the high-res Qt image natively
    bool isInput;
    
    QPixmap cvMatToPixmap(const cv::Mat& img);
    void updateDisplay(); // Handles the dynamic smooth scaling
};

#endif