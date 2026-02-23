#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <opencv2/opencv.hpp>

class ImagePanel : public QWidget {
    Q_OBJECT
public:
    explicit ImagePanel(const QString& title, bool isInput, QWidget *parent = nullptr);

    // ======= ADD THESE MISSING FUNCTIONS =======
    cv::Mat getImage() const;                // Returns the current image
    void displayImage(const cv::Mat& img);   // Shows an image in the panel
    void clear();                            // Resets the panel
    // ===========================================

private slots:
    void handleLoadImage(); // Connected to the "Load" button

private:
    QLabel* imageDisplay;
    cv::Mat currentImage; // Stores the actual OpenCV data
    bool isInput;
    
    // Helper to convert OpenCV to Qt format
    QPixmap cvMatToPixmap(const cv::Mat& img);
};

#endif
