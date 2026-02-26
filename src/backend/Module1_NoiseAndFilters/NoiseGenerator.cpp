#include "NoiseGenerator.h"

cv::Mat NoiseGenerator::addGaussianNoise(const cv::Mat& input, double stddev)
{
    cv::Mat noise(input.size(), input.type());
    cv::randn(noise, 0, stddev);

    cv::Mat output;
    const int signedType = CV_MAKETYPE(CV_16S, input.channels());
    input.convertTo(output, signedType);
    noise.convertTo(noise, signedType);

    cv::addWeighted(output, 1.0, noise, 1.0, 0, output);
    output.convertTo(output, input.type());

    return output;
}

cv::Mat NoiseGenerator::addUniformNoise(const cv::Mat& input, int intensity)
{
    cv::Mat noise(input.size(), input.type());
    cv::randu(noise, 0, intensity);

    cv::Mat output;
    cv::add(input, noise, output);

    return output;
}

cv::Mat NoiseGenerator::addSaltPepperNoise(const cv::Mat& input, double amount)
{
    cv::Mat output = input.clone();
    int numPixels = static_cast<int>(amount * input.total());

    for (int i = 0; i < numPixels; i++)
    {
        int row = rand() % input.rows;
        int col = rand() % input.cols;

        if (output.channels() == 1) {
            output.at<uchar>(row, col) = (rand() % 2) ? 255 : 0;
        } else {
            if (rand() % 2)
                output.at<cv::Vec3b>(row, col) = cv::Vec3b(255,255,255);
            else
                output.at<cv::Vec3b>(row, col) = cv::Vec3b(0,0,0);
        }
    }

    return output;
}