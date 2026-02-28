#include "ImageEqualizer.h"
#include <opencv2/opencv.hpp>

// Compute grayscale histogram
cv::Mat ImageEqualizer::grayScale_histogram(const cv::Mat& image, int min_range, int max_range) {
    cv::Mat histogram;
    int histSize = 256;
    float range[] = { (float)min_range, (float)max_range };
    const float* histRange = { range };
    cv::calcHist(&image, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange);
    return histogram;
}

// Compute RGB histogram
void ImageEqualizer::rgb_histogram(const cv::Mat& image,
                                    cv::Mat& blue_hist, cv::Mat& green_hist, cv::Mat& red_hist,
                                    int min_range, int max_range) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    int histSize = 256;
    float range[] = { (float)min_range, (float)max_range };
    const float* histRange = { range };

    cv::calcHist(&channels[0], 1, 0, cv::Mat(), blue_hist,  1, &histSize, &histRange);
    cv::calcHist(&channels[1], 1, 0, cv::Mat(), green_hist, 1, &histSize, &histRange);
    cv::calcHist(&channels[2], 1, 0, cv::Mat(), red_hist,   1, &histSize, &histRange);
}

// Get histogram (grayscale or RGB)
cv::Mat ImageEqualizer::get_histogram(const cv::Mat& image, int min_range, int max_range) {
    if (image.channels() == 1)
        return grayScale_histogram(image, min_range, max_range);
    else {
        cv::Mat b, g, r;
        rgb_histogram(image, b, g, r, min_range, max_range);
        return b; // returns blue channel histogram as default
    }
}

// Compute grayscale CDF
cv::Mat ImageEqualizer::grayScale_cdf(const cv::Mat& image, int min_range, int max_range) {
    cv::Mat hist = grayScale_histogram(image, min_range, max_range);
    cv::Mat cdf(hist.size(), CV_32F);

    float cumsum = 0;
    double histMax = 0;
    cv::minMaxLoc(hist, nullptr, &histMax);

    for (int i = 0; i < 256; i++) {
        cumsum += hist.at<float>(i);
        cdf.at<float>(i) = cumsum;
    }

    // Normalize: cdf * histMax / cdf_max
    float cdfMax = cdf.at<float>(255);
    cdf = cdf * (float)histMax / cdfMax;

    return cdf;
}

// Compute RGB CDF
void ImageEqualizer::rgb_cdf(const cv::Mat& image,
                              cv::Mat& blue_cdf_out, cv::Mat& green_cdf_out, cv::Mat& red_cdf_out,
                              int min_range, int max_range) {
    cv::Mat blue_hist, green_hist, red_hist;
    rgb_histogram(image, blue_hist, green_hist, red_hist, min_range, max_range);

    auto computeCDF = [](const cv::Mat& hist, cv::Mat& cdf_out) {
        cdf_out = cv::Mat(hist.size(), CV_32F);
        float cumsum = 0;
        double histMax = 0;
        cv::minMaxLoc(hist, nullptr, &histMax);

        for (int i = 0; i < 256; i++) {
            cumsum += hist.at<float>(i);
            cdf_out.at<float>(i) = cumsum;
        }

        float cdfMax = cdf_out.at<float>(255);
        cdf_out = cdf_out * (float)histMax / cdfMax;
    };

    computeCDF(blue_hist,  blue_cdf_out);
    computeCDF(green_hist, green_cdf_out);
    computeCDF(red_hist,   red_cdf_out);
}

// Get CDF (grayscale or RGB)
cv::Mat ImageEqualizer::get_cdf(const cv::Mat& image, int min_range, int max_range) {
    if (image.channels() == 1)
        return grayScale_cdf(image, min_range, max_range);
    else {
        cv::Mat b, g, r;
        rgb_cdf(image, b, g, r, min_range, max_range);
        return b; // returns blue channel CDF as default
    }
}

// Equalize grayscale image
cv::Mat ImageEqualizer::equalize_grayScale(const cv::Mat& image) {
    cv::Mat hist = grayScale_histogram(image);
    cv::Mat cdf(256, 1, CV_32F);

    float cumsum = 0;
    for (int i = 0; i < 256; i++) {
        cumsum += hist.at<float>(i);
        cdf.at<float>(i) = cumsum;
    }

    // Mask zeros and normalize to 0-255
    float cdfMin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf.at<float>(i) > 0) {
            cdfMin = cdf.at<float>(i);
            break;
        }
    }
    float cdfMax = cdf.at<float>(255);
    if (cdfMax == cdfMin)
    return image.clone();

    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        float val = cdf.at<float>(i);
        if (val == 0)
            lut.at<uchar>(i) = 0;
        else
            lut.at<uchar>(i) = cv::saturate_cast<uchar>((val - cdfMin) * 255.0f / (cdfMax - cdfMin));
    }

    cv::Mat equalized;
    cv::LUT(image, lut, equalized);
    return equalized;
}

// Equalize RGB image
cv::Mat ImageEqualizer::equalize_rgb(const cv::Mat& image) {
    cv::Mat blue_cdf_mat, green_cdf_mat, red_cdf_mat;
    rgb_cdf(image, blue_cdf_mat, green_cdf_mat, red_cdf_mat);

    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    
auto buildLUT = [](const cv::Mat& cdf) -> cv::Mat {
    cv::Mat lut(1, 256, CV_8U);
    float cdfMin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf.at<float>(i) > 0) { cdfMin = cdf.at<float>(i); break; }
    }
    float cdfMax = cdf.at<float>(255);
    if (cdfMax == cdfMin)
        return cv::Mat(1, 256, CV_8U, cv::Scalar(0));

    for (int i = 0; i < 256; i++) {
        float val = cdf.at<float>(i);
        lut.at<uchar>(i) = (val == 0) ? 0 :
            cv::saturate_cast<uchar>((val - cdfMin) * 255.0f / (cdfMax - cdfMin));
    }
    return lut;
};
 

    cv::Mat lut_blue  = buildLUT(blue_cdf_mat);
    cv::Mat lut_green = buildLUT(green_cdf_mat);
    cv::Mat lut_red   = buildLUT(red_cdf_mat);

    cv::Mat result_blue, result_green, result_red;
    cv::LUT(channels[0], lut_blue,  result_blue);
    cv::LUT(channels[1], lut_green, result_green);
    cv::LUT(channels[2], lut_red,   result_red);

    cv::Mat equalized;
    cv::merge(std::vector<cv::Mat>{result_blue, result_green, result_red}, equalized);
    return equalized;
}

// Equalize image (grayscale or RGB)
cv::Mat ImageEqualizer::equalize_image(const cv::Mat& image) {
    if (image.channels() == 1)
        return equalize_grayScale(image);
    else
        return equalize_rgb(image);
}
