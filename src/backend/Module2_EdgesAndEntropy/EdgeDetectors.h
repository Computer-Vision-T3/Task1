#ifndef EDGEDETECTORS_H
#define EDGEDETECTORS_H

#include <opencv2/opencv.hpp>
#include <vector>

class EdgeDetectors {
public:
    // Returns a vector containing {X Gradient, Y Gradient, Magnitude}
    static std::vector<cv::Mat> applySobel(const cv::Mat& input);
    static std::vector<cv::Mat> applyPrewitt(const cv::Mat& input);
    static std::vector<cv::Mat> applyRoberts(const cv::Mat& input);
    
    // Canny only outputs a single binary map
    static cv::Mat applyCanny(const cv::Mat& input, double lowerThresh, double upperThresh);

private:
    static cv::Mat convolve(const cv::Mat& input, const std::vector<std::vector<int>>& kernel);
    // Removed generatePreview() as we now return vectors instead of concatenating.
};

#endif // EDGEDETECTORS_H