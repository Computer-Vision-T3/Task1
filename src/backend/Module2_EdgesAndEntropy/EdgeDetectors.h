#ifndef EDGEDETECTORS_H
#define EDGEDETECTORS_H

#include <opencv2/opencv.hpp>
#include <vector>

class EdgeDetectors {
public:
    // The main entry points for the AppController
    static cv::Mat applySobel(const cv::Mat& input);
    static cv::Mat applyPrewitt(const cv::Mat& input);
    static cv::Mat applyRoberts(const cv::Mat& input);
    static cv::Mat applyCanny(const cv::Mat& input, double lowerThresh, double upperThresh);

private:
    // The manual math engine (No cv::filter2D allowed!)
    static cv::Mat convolve(const cv::Mat& input, const std::vector<std::vector<int>>& kernel);
    
    // Calculates magnitude and glues X, Y, and Mag together for the required preview
    static cv::Mat generatePreview(const cv::Mat& gradX, const cv::Mat& gradY);
};

#endif // EDGEDETECTORS_H