#include "EdgeDetectors.h"
#include <cmath>

cv::Mat EdgeDetectors::convolve(const cv::Mat& input, const std::vector<std::vector<int>>& kernel) {
    int kRows = kernel.size();
    int kCols = kernel[0].size();
    int padY = kRows / 2;
    int padX = kCols / 2;

    cv::Mat output = cv::Mat::zeros(input.size(), CV_32F);

    for (int y = padY; y < input.rows - padY; ++y) {
        for (int x = padX; x < input.cols - padX; ++x) {
            float sum = 0.0f;
            for (int ky = -padY; ky <= padY; ++ky) {
                for (int kx = -padX; kx <= padX; ++kx) {
                    int pixel = input.at<uchar>(y + ky, x + kx);
                    int weight = kernel[ky + padY][kx + padX];
                    sum += pixel * weight;
                }
            }
            output.at<float>(y, x) = sum;
        }
    }
    return output;
}

// Private helper to process the 3 outputs for the manual convolution methods
static std::vector<cv::Mat> processGradients(const cv::Mat& gradX, const cv::Mat& gradY) {
    cv::Mat magnitude = cv::Mat::zeros(gradX.size(), CV_32F);
    for (int y = 0; y < gradX.rows; ++y) {
        for (int x = 0; x < gradX.cols; ++x) {
            float gx = gradX.at<float>(y, x);
            float gy = gradY.at<float>(y, x);
            magnitude.at<float>(y, x) = std::sqrt(gx*gx + gy*gy);
        }
    }

    cv::Mat dispX, dispY, dispMag;
    cv::convertScaleAbs(gradX, dispX); 
    cv::convertScaleAbs(gradY, dispY);
    cv::convertScaleAbs(magnitude, dispMag);

    return {dispX, dispY, dispMag};
}

std::vector<cv::Mat> EdgeDetectors::applySobel(const cv::Mat& input) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    std::vector<std::vector<int>> sobelX = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    std::vector<std::vector<int>> sobelY = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    cv::Mat gradX = convolve(gray, sobelX);
    cv::Mat gradY = convolve(gray, sobelY);

    return processGradients(gradX, gradY);
}

std::vector<cv::Mat> EdgeDetectors::applyPrewitt(const cv::Mat& input) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    std::vector<std::vector<int>> prewittX = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    std::vector<std::vector<int>> prewittY = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};

    cv::Mat gradX = convolve(gray, prewittX);
    cv::Mat gradY = convolve(gray, prewittY);

    return processGradients(gradX, gradY);
}

std::vector<cv::Mat> EdgeDetectors::applyRoberts(const cv::Mat& input) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    std::vector<std::vector<int>> robertsX = {{1, 0, 0}, {0, -1, 0}, {0, 0, 0}};
    std::vector<std::vector<int>> robertsY = {{0, 1, 0}, {-1, 0, 0}, {0, 0, 0}};

    cv::Mat gradX = convolve(gray, robertsX);
    cv::Mat gradY = convolve(gray, robertsY);

    return processGradients(gradX, gradY);
}

cv::Mat EdgeDetectors::applyCanny(const cv::Mat& input, double lowerThresh, double upperThresh) {
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    cv::Mat edges;
    cv::Canny(gray, edges, lowerThresh, upperThresh);
    
    return edges; 
}