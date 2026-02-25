#include "EntropyCalculator.h"
#include <vector>
#include <cmath>

// ==========================================
// 1. MANUAL ENTROPY CALCULATION
// ==========================================
double EntropyCalculator::calculate(const cv::Mat& input) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) {
        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
    }

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

// ==========================================
// 2. PLOT DISTRIBUTION GRAPH
// ==========================================
cv::Mat EntropyCalculator::plotHistogram(const cv::Mat& input) {
    cv::Mat gray;
    if (input.channels() == 3) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else gray = input.clone();

    // 1. Calculate the standard OpenCV histogram
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    cv::Mat hist;
    cv::calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

    // 2. Setup a dark canvas for the graph
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double) hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(40, 40, 40));

    // 3. Normalize the data so the highest peak fits on the screen
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // 4. Draw the actual graph line
    for( int i = 1; i < histSize; i++ ) {
        cv::line(histImage, 
                 cv::Point(bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1))),
                 cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
                 cv::Scalar(0, 255, 255), 2, 8, 0); // Yellow/Cyan line
    }
    return histImage;
}