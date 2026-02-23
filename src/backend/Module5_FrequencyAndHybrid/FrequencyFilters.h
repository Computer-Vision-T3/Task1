#ifndef FREQUENCYFILTERS_H
#define FREQUENCYFILTERS_H

#include <opencv2/opencv.hpp>

class FrequencyFilters {
public:
    enum FilterType { LOW_PASS, HIGH_PASS };
    static cv::Mat applyFilter(const cv::Mat& input, float D0, FilterType type);

private:
    static void shiftDFT(cv::Mat& f);
    static cv::Mat createGaussianMask(cv::Size size, float D0, bool isLowPass);
};

#endif
