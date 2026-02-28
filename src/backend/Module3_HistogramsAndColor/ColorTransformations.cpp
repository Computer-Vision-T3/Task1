#include "ColorTransformations.h"
#include "HistogramTools.h"

cv::Mat ColorTransformations::convertToGray(const cv::Mat& input) {
    cv::Mat gray;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

std::vector<cv::Mat> ColorTransformations::analyzeRGB(const cv::Mat& input) {
    std::vector<cv::Mat> bgr_planes;
    cv::split(input, bgr_planes);

    cv::Mat b_hist, g_hist, r_hist, b_cdf, g_cdf, r_cdf;

    HistogramTools::getHistogramAndCDF(bgr_planes[0], b_hist, b_cdf);
    HistogramTools::getHistogramAndCDF(bgr_planes[1], g_hist, g_cdf);
    HistogramTools::getHistogramAndCDF(bgr_planes[2], r_hist, r_cdf);

    // Return the generated plots as actual images
    cv::Mat bPlot = HistogramTools::plotHistogram(b_hist, b_cdf, cv::Scalar(255, 0, 0));
    cv::Mat gPlot = HistogramTools::plotHistogram(g_hist, g_cdf, cv::Scalar(0, 255, 0));
    cv::Mat rPlot = HistogramTools::plotHistogram(r_hist, r_cdf, cv::Scalar(0, 0, 255));

    return {bPlot, gPlot, rPlot};
}