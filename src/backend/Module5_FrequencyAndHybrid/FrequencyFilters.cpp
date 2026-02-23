#include "FrequencyFilters.h"

void FrequencyFilters::shiftDFT(cv::Mat& f) {
    int cx = f.cols / 2;
    int cy = f.rows / 2;
    cv::Mat q0(f, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(f, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(f, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(f, cv::Rect(cx, cy, cx, cy));
    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);
}

cv::Mat FrequencyFilters::createGaussianMask(cv::Size size, float D0, bool isLowPass) {
    cv::Mat mask = cv::Mat::zeros(size, CV_32F);
    int crow = size.height / 2;
    int ccol = size.width / 2;
    for (int i = 0; i < size.height; i++) {
        for (int j = 0; j < size.width; j++) {
            float D = std::sqrt(std::pow(i - crow, 2) + std::pow(j - ccol, 2));
            float val = std::exp(-(D * D) / (2 * D0 * D0));
            mask.at<float>(i, j) = isLowPass ? val : (1.0f - val);
        }
    }
    cv::Mat channels[] = {mask, mask};
    cv::Mat complexMask;
    cv::merge(channels, 2, complexMask);
    return complexMask;
}

cv::Mat FrequencyFilters::applyFilter(const cv::Mat& input, float D0, FilterType type) {
    if (input.empty()) return cv::Mat();
    
    cv::Mat gray;
    if (input.channels() > 1) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY); else gray = input.clone();

    // Force dimensions to be even for DFT
    int r = gray.rows & -2;
    int c = gray.cols & -2;
    gray = gray(cv::Rect(0, 0, c, r)).clone();
    gray.convertTo(gray, CV_32F);

    // DFT
    cv::Mat planes[] = {gray, cv::Mat::zeros(gray.size(), CV_32F)};
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI);
    shiftDFT(complexI);

    // Apply Mask
    cv::Mat mask = createGaussianMask(gray.size(), D0, (type == LOW_PASS));
    cv::multiply(complexI, mask, complexI);

    // Inverse DFT
    shiftDFT(complexI);
    cv::idft(complexI, complexI);
    cv::split(complexI, planes);
    cv::Mat result;
    cv::magnitude(planes[0], planes[1], result);

    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result.convertTo(result, CV_8U);
    return result;
}
