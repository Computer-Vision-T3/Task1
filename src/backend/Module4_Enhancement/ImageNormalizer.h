#pragma once
#include <opencv2/opencv.hpp>

class ImageNormalizer {
public:
    // Normalize grayscale image (returns float 0-1)
    cv::Mat normalize_grayscale(const cv::Mat& image);

    // Normalize RGB image (returns float 0-1 per channel)
    cv::Mat normalize_rgb(const cv::Mat& image);

    // Normalize image (grayscale or RGB)
    cv::Mat normalize_image(const cv::Mat& image);
};
