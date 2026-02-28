#include "EntropyCalculator.h"
#include <vector>
#include <cmath>

double EntropyCalculator::calculate(const cv::Mat& input) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) { cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY); }

    std::vector<double> pixelCounts(256, 0.0);
    for (int y = 0; y < gray.rows; ++y) {
        for (int x = 0; x < gray.cols; ++x) {
            int pixelValue = gray.at<uchar>(y, x);
            pixelCounts[pixelValue]++;
        }
    }

    double totalPixels = gray.rows * gray.cols;
    double entropy = 0.0;

    for (int i = 0; i < 256; ++i) {
        if (pixelCounts[i] > 0) {
            double probability = pixelCounts[i] / totalPixels;
            entropy -= probability * std::log2(probability);
        }
    }
    return entropy;
}

cv::Mat EntropyCalculator::plotHistogram(const cv::Mat& input) {
    cv::Mat gray;
    if (input.channels() == 3) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else gray = input.clone();

    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    cv::Mat hist;
    cv::calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double) hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(22, 24, 29));

    // 1. Grid
    for (int i = 0; i <= hist_h; i += 50)
        cv::line(histImage, cv::Point(0, i), cv::Point(hist_w, i), cv::Scalar(45, 48, 55), 1);
    for (int i = 0; i <= hist_w; i += 64)
        cv::line(histImage, cv::Point(i, 0), cv::Point(i, hist_h), cv::Scalar(45, 48, 55), 1);

    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // 2. Neon Magenta Fill
    cv::Scalar color(255, 50, 200); // Hot Pink/Magenta
    cv::Scalar fillC(color[0]*0.4, color[1]*0.4, color[2]*0.4); 
    
    for (int i = 1; i < 256; i++) {
        int h = cvRound(hist.at<float>(i));
        int x = bin_w * i;
        cv::line(histImage, cv::Point(x, hist_h), cv::Point(x, hist_h - h), fillC, bin_w, cv::LINE_8);
    }

    // 3. Thick Anti-Aliased Outline
    for( int i = 1; i < histSize; i++ ) {
        cv::line(histImage, 
                 cv::Point(bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1))),
                 cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
                 color, 2, cv::LINE_AA); 
    }
    return histImage;
}