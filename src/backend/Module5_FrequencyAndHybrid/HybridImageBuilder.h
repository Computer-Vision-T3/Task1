#ifndef HYBRIDIMAGEBUILDER_H
#define HYBRIDIMAGEBUILDER_H

#include <opencv2/opencv.hpp>

class HybridImageBuilder {
public:
    // sigma determines the "cutoff". Standard value is around 5-15.
    static cv::Mat createHybrid(cv::Mat img1, cv::Mat img2, int sigma);
};

#endif
