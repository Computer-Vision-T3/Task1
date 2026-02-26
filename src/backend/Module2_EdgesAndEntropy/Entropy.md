
# Part 1: Image Entropy Analysis (Task 7)

This module calculates the **Shannon Entropy** of an image. In information theory and computer vision, entropy is a statistical measure of randomness, variance, or "information content." To comply with strict "from-scratch" constraints, the core mathematical calculation is performed manually using standard C++ arrays and loops, completely bypassing OpenCV's built-in histogram calculators for the data extraction.

## The Mathematical Pipeline: How It Works

An image with low entropy consists of large areas of uniform color (like a solid blue sky), meaning it contains very little structural information. An image with high entropy has wildly varying pixel intensities (like a dense forest or television static), meaning it contains a massive amount of information.

To calculate this, we must determine the probability of every single shade of gray (from 0 to 255) appearing in the image, and then plug those probabilities into the Shannon Entropy equation.

### Mathematical Example: Manual Calculation

Imagine a tiny 4x4 grayscale image ($I$) representing a sharp black-and-white checkerboard:


$$I = \begin{bmatrix} 0 & 255 & 0 & 255 \\ 255 & 0 & 255 & 0 \\ 0 & 255 & 0 & 255 \\ 255 & 0 & 255 & 0 \end{bmatrix}$$

**Step 1: Count the Pixel Frequencies (Manual Histogram)**
We count how many times each pixel intensity occurs. Our image has exactly 16 pixels.

* Count of `0` (Black): 8 pixels
* Count of `255` (White): 8 pixels
* Count of all other values (1 through 254): 0 pixels

**Step 2: Calculate Probabilities ($p_i$)**
The probability of a pixel value occurring is its count divided by the total number of pixels.

* $p_0 = 8 / 16 = 0.5$
* $p_{255} = 8 / 16 = 0.5$

**Step 3: Apply the Shannon Entropy Formula ($H$)**
We sum the probabilities using a base-2 logarithm. If a probability is 0, we skip it (since $\log_2(0)$ is undefined).


$$H = -\sum_{i=0}^{255} p_i \log_2(p_i)$$

$$H = - (p_0 \log_2(p_0) + p_{255} \log_2(p_{255}))$$

$$H = - (0.5 \times \log_2(0.5) + 0.5 \times \log_2(0.5))$$

$$H = - (0.5 \times -1 + 0.5 \times -1)$$

$$H = - (-0.5 - 0.5) = 1.0$$


*(An entropy of 1.0 is very low, which makes perfect sense because our image only contains two colors!)*

---

## Explicit Parameters & Outputs

**Parameters:**

* **Input Image:** A standard RGB or Grayscale `cv::Mat`. No additional sliders or user inputs are required for this filter.

**Explicit Outputs & Meanings:**

1. **The Entropy Value (Sidebar Text):** A floating-point number formatted to 4 decimal places.
* *Low (< 4.0):* Flat, uniform, low-contrast images.
* *Moderate (4.0 - 7.0):* Standard, naturally lit photographs.
* *High (> 7.0):* Highly complex, noisy, or intensely textured images.


2. **Pixel Distribution Graph (Main Panel):** A generated 2D line graph visualizing the spread of pixel intensities from 0 to 255. This visually proves the entropy calculation. A narrow, spiked graph yields low entropy, while a wide, flat graph yields high entropy.

---

# Part 2: Code Implementation & Architecture

This section provides a line-by-line breakdown of the `EntropyCalculator` class and its UI integration.

## 1. How the Image is Received (The Routing)

When the user selects "Task 7: Entropy", the `AppController` triggers a custom UI layout. It shrinks the right-side output panel and spawns a third `QTextBrowser` sidebar.

It passes the input image into the calculator twice:

1. `double entropy = EntropyCalculator::calculate(input);` -> To get the raw number for the sidebar.
2. `cv::Mat histGraph = EntropyCalculator::plotHistogram(input);` -> To generate the graph for the center panel.

---

## 2. The Header File: `EntropyCalculator.h`

Just like the edge detectors, this relies on a stateless, `static` utility class.

```cpp
#ifndef ENTROPYCALCULATOR_H
#define ENTROPYCALCULATOR_H

#include <opencv2/opencv.hpp>

class EntropyCalculator {
public:
    // Core math engine: Returns the raw floating-point entropy number.
    static double calculate(const cv::Mat& input);

    // Visualization engine: Returns an OpenCV image containing the plotted distribution graph.
    static cv::Mat plotHistogram(const cv::Mat& input);
};

#endif // ENTROPYCALCULATOR_H

```

---

## 3. The Implementation File: `EntropyCalculator.cpp`

### A. The Core Math Engine: `calculate()`

This function rigidly adheres to the "no library" constraint by avoiding OpenCV's `calcHist` entirely, instead utilizing a standard C++ `std::vector` to count pixels in memory.

```cpp
double EntropyCalculator::calculate(const cv::Mat& input) {
    // 1. PREPROCESSING
    // Entropy is calculated on luminance (brightness), so we force grayscale.
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) {
        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
    }

    // 2. MANUAL FREQUENCY COUNTING
    // We create an array of 256 zeros (representing pixel values 0 through 255).
    std::vector<double> pixelCounts(256, 0.0);
    
    // We loop over every physical pixel in the matrix.
    for (int y = 0; y < gray.rows; ++y) {
        for (int x = 0; x < gray.cols; ++x) {
            // We extract the intensity (0-255) and use it as the index 
            // for our array, incrementing that specific bucket by 1.
            int pixelValue = gray.at<uchar>(y, x);
            pixelCounts[pixelValue]++;
        }
    }

    // 3. PROBABILITY & ENTROPY MATH
    double totalPixels = gray.rows * gray.cols;
    double entropy = 0.0;

    // Loop through our 256 buckets
    for (int i = 0; i < 256; ++i) {
        if (pixelCounts[i] > 0) { // Protect against log2(0) crashes
            
            // Calculate p_i (Count / Total)
            double probability = pixelCounts[i] / totalPixels;
            
            // Apply the Shannon formula: H -= p * log2(p)
            entropy -= probability * std::log2(probability);
        }
    }
    return entropy;
}

```

### B. The Visualization Engine: `plotHistogram()`

While the actual math is done manually above, we use OpenCV's built-in drawing tools here to generate a slick, neon graph for the user interface.

```cpp
cv::Mat EntropyCalculator::plotHistogram(const cv::Mat& input) {
    // Preprocessing (Grayscale conversion)
    cv::Mat gray;
    if (input.channels() == 3) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else gray = input.clone();

    // 1. EXTRACT DATA FOR PLOTTING
    // We use OpenCV's calcHist purely to generate the array needed for the graph line.
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    cv::Mat hist;
    cv::calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

    // 2. SETUP THE UI CANVAS
    // Create a 512x400 dark gray background (Scalar 40, 40, 40)
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double) hist_w / histSize); // Width of each graph step (512 / 256 = 2 pixels)
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(40, 40, 40));

    // 3. NORMALIZE THE DATA
    // A photo might have 500,000 black pixels. We scale the data so the highest
    // peak perfectly touches the top of our 400-pixel-tall canvas.
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    // 4. DRAW THE GRAPH
    // Iterate from 1 to 255, drawing a connecting line from the previous point to the current point.
    for( int i = 1; i < histSize; i++ ) {
        cv::line(histImage, 
                 // Point 1 (X_prev, Y_prev)
                 cv::Point(bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1))),
                 // Point 2 (X_current, Y_current)
                 cv::Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
                 // Color (Yellow/Cyan), Thickness (2), LineType (8-connected)
                 cv::Scalar(0, 255, 255), 2, 8, 0); 
    }
    return histImage;
}

```

## 🛠️ How to Modify or Extend This Code

* **To change the graph color:** In `plotHistogram()`, locate `cv::Scalar(0, 255, 255)`. This is in **BGR** format (Blue, Green, Red). Change it to `cv::Scalar(0, 255, 0)` for neon green or `cv::Scalar(255, 0, 0)` for pure blue.
* **To alter the Sidebar text/logic:** Open `AppController.cpp`, locate `if (taskIndex == 7)`. You can freely modify the Qt `QString` HTML blocks to change font sizes, colors, or tweak the thresholds (currently set at `< 4.0` and `> 7.0`) that trigger the plain-English explanations.