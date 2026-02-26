#include "HistogramTools.h"

void HistogramTools::getHistogramAndCDF(const cv::Mat& input, cv::Mat& hist, cv::Mat& cdf) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    // Calculate Histogram
    cv::calcHist(&input, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // Calculate Distribution Curve (CDF)
    cdf = hist.clone();
    for (int i = 1; i < histSize; i++) {
        cdf.at<float>(i) += cdf.at<float>(i - 1);
    }
    
    // Normalize CDF for mapping/equalization
    cv::normalize(cdf, cdf, 0, histSize, cv::NORM_MINMAX);
}

cv::Mat HistogramTools::plotHistogram(const cv::Mat& hist, const cv::Mat& cdf, cv::Scalar color) {
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / 256);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::Mat n_hist;
    cv::normalize(hist, n_hist, 0, histImage.rows, cv::NORM_MINMAX);

    for (int i = 1; i < 256; i++) {
        // Draw Histogram (Solid Lines)
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(n_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(n_hist.at<float>(i))), color, 2);

        // Draw Distribution Curve (Dashed/Thin Lines)
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(cdf.at<float>(i - 1) * hist_h / 256)),
            cv::Point(bin_w * (i), hist_h - cvRound(cdf.at<float>(i) * hist_h / 256)), cv::Scalar(255, 255, 255), 1);
    }
    return histImage;
}