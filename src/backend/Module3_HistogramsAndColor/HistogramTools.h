#ifndef HISTOGRAM_TOOLS_H
#define HISTOGRAM_TOOLS_H

#include <opencv2/opencv.hpp>
#include <vector>

class HistogramTools {
public:
    // Task 4: Calculate Histogram and CDF for a single channel (Gray)
    static void getHistogramAndCDF(const cv::Mat& input, cv::Mat& hist, cv::Mat& cdf);

    // Task 4 & 8: Helper to visualize the histogram and curve
    static cv::Mat plotHistogram(const cv::Mat& hist, const cv::Mat& cdf, cv::Scalar color);
};

#endif