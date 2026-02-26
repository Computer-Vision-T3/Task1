#pragma once
#include <opencv2/opencv.hpp>

class LowPassFilters
{
public:
    static cv::Mat applyAverage(const cv::Mat& input, int kernelSize);
    static cv::Mat applyGaussian(const cv::Mat& input, int kernelSize);
    static cv::Mat applyMedian(const cv::Mat& input, int kernelSize);
};