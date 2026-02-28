#include "HistogramTools.h"

void HistogramTools::getHistogramAndCDF(const cv::Mat& input, cv::Mat& hist, cv::Mat& cdf) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    cv::calcHist(&input, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    cdf = hist.clone();
    for (int i = 1; i < histSize; i++) {
        cdf.at<float>(i) += cdf.at<float>(i - 1);
    }
    cv::normalize(cdf, cdf, 0, histSize, cv::NORM_MINMAX);
}

cv::Mat HistogramTools::plotHistogram(const cv::Mat& hist, const cv::Mat& cdf, cv::Scalar color) {
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / 256);
    
    // Very dark blue/grey aesthetic background
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(22, 24, 29));

    // 1. Draw subtle background grid
    for (int i = 0; i <= hist_h; i += 50)
        cv::line(histImage, cv::Point(0, i), cv::Point(hist_w, i), cv::Scalar(45, 48, 55), 1);
    for (int i = 0; i <= hist_w; i += 64)
        cv::line(histImage, cv::Point(i, 0), cv::Point(i, hist_h), cv::Scalar(45, 48, 55), 1);

    cv::Mat n_hist;
    cv::normalize(hist, n_hist, 0, histImage.rows, cv::NORM_MINMAX);

    // 2. Artistic "Filled" area using vertical lines
    cv::Scalar fillC(color[0]*0.4, color[1]*0.4, color[2]*0.4); // Darker tone of the line color
    for (int i = 1; i < 256; i++) {
        int h = cvRound(n_hist.at<float>(i));
        int x = bin_w * i;
        cv::line(histImage, cv::Point(x, hist_h), cv::Point(x, hist_h - h), fillC, bin_w, cv::LINE_8);
    }

    // 3. Thick Anti-Aliased bright outline & CDF Curve
    for (int i = 1; i < 256; i++) {
        // Histogram Curve
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(n_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(n_hist.at<float>(i))), color, 2, cv::LINE_AA);

        // CDF Curve (Bright White)
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(cdf.at<float>(i - 1) * hist_h / 256)),
            cv::Point(bin_w * (i), hist_h - cvRound(cdf.at<float>(i) * hist_h / 256)), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    }
    return histImage;
}