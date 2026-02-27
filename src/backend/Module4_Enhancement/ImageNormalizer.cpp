#include "ImageNormalizer.h"
#include <opencv2/opencv.hpp>

// Normalize grayscale image (returns float 0-1)
cv::Mat ImageNormalizer::normalize_grayscale(const cv::Mat& image) {
    cv::Mat normalized;
    image.convertTo(normalized, CV_32F);

    double minVal, maxVal;
    cv::minMaxLoc(normalized, &minVal, &maxVal);

    double range = maxVal - minVal;

    if (range == 0)
        return cv::Mat::zeros(image.size(), CV_32F);

    normalized = (normalized - minVal) / range;  // Result is float 0-1
    return normalized;
}

// Normalize RGB image (returns float 0-1 per channel)
cv::Mat ImageNormalizer::normalize_rgb(const cv::Mat& image) {
    cv::Mat normalized;
    image.convertTo(normalized, CV_32F);

    std::vector<cv::Mat> channels;
    cv::split(normalized, channels);

    for (int i = 0; i < 3; i++) {
        double minVal, maxVal;
        cv::minMaxLoc(channels[i], &minVal, &maxVal);

        double range = maxVal - minVal;

        if (range == 0)
            channels[i] = cv::Mat::zeros(channels[i].size(), CV_32F);
        else
            channels[i] = (channels[i] - minVal) / range;  // Result is float 0-1
    }

    cv::merge(channels, normalized);
    return normalized;
}

// Normalize image (grayscale or RGB)
cv::Mat ImageNormalizer::normalize_image(const cv::Mat& image) {
    if (image.channels() == 1)
        return normalize_grayscale(image);
    else if (image.channels() == 3)
        return normalize_rgb(image);
    return image;  // Return unchanged if format is unknown
}
