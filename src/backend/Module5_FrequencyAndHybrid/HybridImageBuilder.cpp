#include "HybridImageBuilder.h"

// Helper function to shift quadrants (similar to np.fft.fftshift)
// Removed the cropping logic from here to prevent size mismatches
void shiftDFT(cv::Mat& f) {
    int cx = f.cols / 2;
    int cy = f.rows / 2;
    cv::Mat q0(f, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(f, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(f, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(f, cv::Rect(cx, cy, cx, cy));
    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);
}

// Helper to create a Gaussian Mask (Frequency Domain)
cv::Mat createGaussianMask(cv::Size size, float D0, bool isLowPass) {
    cv::Mat mask = cv::Mat::zeros(size, CV_32F);
    int crow = size.height / 2;
    int ccol = size.width / 2;

    for (int i = 0; i < size.height; i++) {
        for (int j = 0; j < size.width; j++) {
            float D = std::sqrt(std::pow(i - crow, 2) + std::pow(j - ccol, 2));
            float val = std::exp(-(D * D) / (2 * D0 * D0));
            mask.at<float>(i, j) = isLowPass ? val : (1.0f - val);
        }
    }
    cv::Mat channels[] = {mask, mask};
    cv::Mat complexMask;
    cv::merge(channels, 2, complexMask);
    return complexMask;
}

cv::Mat processFrequency(cv::Mat img, float D0, bool isLowPass) {
    // 1. DFT
    cv::Mat planes[] = {img, cv::Mat::zeros(img.size(), CV_32F)};
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg);
    shiftDFT(complexImg);

    // 2. Apply Mask (Size now matches perfectly)
    cv::Mat mask = createGaussianMask(img.size(), D0, isLowPass);
    cv::multiply(complexImg, mask, complexImg);

    // 3. Inverse DFT
    shiftDFT(complexImg);
    cv::idft(complexImg, complexImg);
    
    // 4. Calculate Magnitude
    cv::split(complexImg, planes);
    cv::Mat result;
    cv::magnitude(planes[0], planes[1], result);
    return result;
}

cv::Mat HybridImageBuilder::createHybrid(cv::Mat img1, cv::Mat img2, int sigma) {
    if (img1.empty() || img2.empty()) return cv::Mat();

    cv::Mat gray1, gray2;
    if (img1.channels() > 1) cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY); else gray1 = img1.clone();
    if (img2.channels() > 1) cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY); else gray2 = img2.clone();

    // Resize img2 to match img1
    if (gray1.size() != gray2.size()) cv::resize(gray2, gray2, gray1.size());

    // CRITICAL FIX: Ensure dimensions are EVEN before starting
    // This prevents the "Sizes do not match" crash in DFT operations
    int evenRows = gray1.rows & -2;
    int evenCols = gray1.cols & -2;
    cv::Rect roi(0, 0, evenCols, evenRows);
    gray1 = gray1(roi).clone();
    gray2 = gray2(roi).clone();

    gray1.convertTo(gray1, CV_32F);
    gray2.convertTo(gray2, CV_32F);

    // Use sigma as cutoff (D0)
    float D0_lp = (float)sigma; 
    float D0_hp = (float)sigma * 1.5f; 

    cv::Mat lp_img = processFrequency(gray1, D0_lp, true);
    cv::Mat hp_img = processFrequency(gray2, D0_hp, false);

    // Ensure results match before adding
    if (lp_img.size() != hp_img.size()) cv::resize(hp_img, hp_img, lp_img.size());

    // Combine
    cv::Mat hybrid = lp_img + hp_img;

    // Normalize for display
    cv::Mat result;
    cv::normalize(hybrid, result, 0, 255, cv::NORM_MINMAX);
    result.convertTo(result, CV_8U);

    return result;
}
