#ifndef COLOR_TRANSFORMATIONS_H
#define COLOR_TRANSFORMATIONS_H

#include <opencv2/opencv.hpp>
#include <vector>

class ColorTransformations {
public:
    static cv::Mat convertToGray(const cv::Mat& input);
    
    // Changed to return a list of 3 plots instead of launching windows
    static std::vector<cv::Mat> analyzeRGB(const cv::Mat& input);
};

#endif