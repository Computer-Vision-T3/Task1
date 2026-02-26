#pragma once
#include<opencv2/opencv.hpp>
class NoiseGenerator
{public:
    static cv::Mat addGaussianNoise(const cv::Mat& input,double stddev);
    static cv::Mat addUniformNoise(const cv::Mat& input, int intensity);
    static cv::Mat addSaltPepperNoise(const cv::Mat& input, double amount);
};