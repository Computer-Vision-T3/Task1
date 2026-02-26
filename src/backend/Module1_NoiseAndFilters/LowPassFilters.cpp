#include "LowPassFilters.h"

cv::Mat LowPassFilters::applyAverage(const cv::Mat& input, int kernelSize)
{
    cv::Mat output;
    cv::blur(input, output, cv::Size(kernelSize, kernelSize));
    return output;
}

cv::Mat LowPassFilters::applyGaussian(const cv::Mat& input, int kernelSize)
{
    cv::Mat output;
    cv::GaussianBlur(input, output, cv::Size(kernelSize, kernelSize), 0);
    return output;
}

cv::Mat LowPassFilters::applyMedian(const cv::Mat& input, int kernelSize)
{
    cv::Mat output;
    cv::medianBlur(input, output, kernelSize);
    return output;
}