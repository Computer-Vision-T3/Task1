#ifndef ENTROPYCALCULATOR_H
#define ENTROPYCALCULATOR_H

#include <opencv2/opencv.hpp>

class EntropyCalculator {
public:
    // Calculates the raw entropy value of the image
    static double calculate(const cv::Mat& input);

    // Creates a beautiful neon histogram graph of the pixel distribution
    static cv::Mat plotHistogram(const cv::Mat& input);
};

#endif // ENTROPYCALCULATOR_H