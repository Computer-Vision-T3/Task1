#ifndef COLOR_TRANSFORMATIONS_H
#define COLOR_TRANSFORMATIONS_H

#include <opencv2/opencv.hpp>

class ColorTransformations {
public:
    // Task 8: Manual conversion from RGB to Gray
    static cv::Mat convertToGray(const cv::Mat& input);
    
    // Task 8: Analyze and return histograms for R, G, and B channels
    static void analyzeRGB(const cv::Mat& input);
};

#endif