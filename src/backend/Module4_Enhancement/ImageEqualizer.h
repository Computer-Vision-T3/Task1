#pragma once
#include <opencv2/opencv.hpp>

class ImageEqualizer {
public:
    // Compute grayscale histogram
    cv::Mat grayScale_histogram(const cv::Mat& image, int min_range = 0, int max_range = 256);

    // Compute RGB histogram
    void rgb_histogram(const cv::Mat& image,
                       cv::Mat& blue_hist, cv::Mat& green_hist, cv::Mat& red_hist,
                       int min_range = 0, int max_range = 256);

    // Get histogram (grayscale or RGB)
    cv::Mat get_histogram(const cv::Mat& image, int min_range = 0, int max_range = 256);

    // Compute grayscale CDF
    cv::Mat grayScale_cdf(const cv::Mat& image, int min_range = 0, int max_range = 256);

    // Compute RGB CDF
    void rgb_cdf(const cv::Mat& image,
                 cv::Mat& blue_cdf, cv::Mat& green_cdf, cv::Mat& red_cdf,
                 int min_range = 0, int max_range = 256);

    // Get CDF (grayscale or RGB)
    cv::Mat get_cdf(const cv::Mat& image, int min_range = 0, int max_range = 256);

    // Equalize grayscale image
    cv::Mat equalize_grayScale(const cv::Mat& image);

    // Equalize RGB image
    cv::Mat equalize_rgb(const cv::Mat& image);

    // Equalize image (grayscale or RGB)
    cv::Mat equalize_image(const cv::Mat& image);
};
