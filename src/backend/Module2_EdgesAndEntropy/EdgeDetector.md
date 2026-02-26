# Part 1: Edge Detection Algorithms (Task 3)

This module identifies boundaries within an image where pixel intensity changes sharply. According to the project requirements, Sobel, Prewitt, and Roberts are built entirely from scratch using a custom manual 2D convolution engine, while the Canny edge detector utilizes the OpenCV library. The output for the manual masks must include previews in the X and Y directions.

## The Mathematical Pipeline: How It Works

An edge in an image is simply a rapid change in pixel intensity (e.g., transitioning from a black pixel at `0` to a white pixel at `255`). To find these changes mathematically, we calculate the **derivative** (the rate of change) of the image pixels.

Because an image is a grid of discrete numbers (not a continuous curve), we approximate this derivative using a process called **2D Convolution**. We slide a small matrix (a "kernel" or "mask") across the image. At every pixel, we multiply the kernel values by the overlapping image pixels and sum them up.

### Mathematical Example: Manual Convolution

Imagine a tiny 3x3 patch of our grayscale image ($I$) that contains a sharp vertical edge (dark on the left, bright on the right):


$$I = \begin{bmatrix} 100 & 100 & 200 \\ 100 & 100 & 200 \\ 100 & 100 & 200 \end{bmatrix}$$

We want to calculate the horizontal gradient ($G_x$) for the center pixel using the **Sobel X kernel** ($K_x$):


$$K_x = \begin{bmatrix} -1 & 0 & 1 \\ -2 & 0 & 2 \\ -1 & 0 & 1 \end{bmatrix}$$

**Step 1: Multiply overlapping values and sum them up**
$G_x = (-1 \times 100) + (0 \times 100) + (1 \times 200) + (-2 \times 100) + (0 \times 100) + (2 \times 200) + (-1 \times 100) + (0 \times 100) + (1 \times 200)$
$G_x = -100 + 0 + 200 - 200 + 0 + 400 - 100 + 0 + 200$
$G_x = 400$
*(A high number means a strong vertical edge was detected!)*

**Step 2: Calculate the vertical gradient ($G_y$)**
If we slide the **Sobel Y kernel** (which looks for horizontal edges) over the exact same image patch:
$G_y = (-1 \times 100) + (-2 \times 100) + (-1 \times 200) + (0 \times \dots) + (1 \times 100) + (2 \times 100) + (1 \times 200)$
$G_y = -100 - 200 - 200 + 0 + 100 + 200 + 200 = 0$
*(A zero means absolutely no horizontal edge was detected, which perfectly matches our image patch).*

**Step 3: Calculate the Final Magnitude ($G$)**
To get the total edge strength regardless of direction, we use the Pythagorean theorem:


$$G = \sqrt{G_x^2 + G_y^2}$$

$$G = \sqrt{400^2 + 0^2} = 400$$

*(Note: Because gradient calculations can result in negative numbers or numbers exceeding 255, the intermediate math is done using 32-bit floats. It is normalized back to standard 8-bit 0-255 pixels for final display).*

---

## 1. The Sobel Operator

**Concept:** Sobel uses two 3x3 derivative masks. It places a higher weight (a value of 2) on the center pixels of the cross. This acts as a slight Gaussian smoothing effect, giving it natural resistance to mild image noise.

**Mathematical Kernels:**


$$G_x = \begin{bmatrix} -1 & 0 & 1 \\ -2 & 0 & 2 \\ -1 & 0 & 1 \end{bmatrix} \quad G_y = \begin{bmatrix} -1 & -2 & -1 \\ 0 & 0 & 0 \\ 1 & 2 & 1 \end{bmatrix}$$

**Parameters:**

* **Kernel Size:** Fixed at 3x3.
* **Padding:** Zero-padding (edges of the image are ignored to prevent out-of-bounds array calculations).

**Explicit Outputs & Meanings:**

1. **$X$ Gradient (Output 1):** Highlights changes along the x-axis. Physically, this image reveals all the **vertical lines** (e.g., the sides of buildings, tree trunks).
2. **$Y$ Gradient (Output 2):** Highlights changes along the y-axis. Physically, this image reveals all the **horizontal lines** (e.g., the horizon, rooftops).
3. **Magnitude (Final Output):** The combined absolute edge strength. This is the complete edge map highlighting both vertical and horizontal structures cleanly.

---

## 2. The Prewitt Operator

**Concept:** Prewitt is mathematically similar to Sobel, but it does not emphasize the center pixel. It applies an equal weight to all pixels in the row or column being evaluated. It is slightly more susceptible to high-frequency noise but offers a more uniform directional response.

**Mathematical Kernels:**


$$G_x = \begin{bmatrix} -1 & 0 & 1 \\ -1 & 0 & 1 \\ -1 & 0 & 1 \end{bmatrix} \quad G_y = \begin{bmatrix} -1 & -1 & -1 \\ 0 & 0 & 0 \\ 1 & 1 & 1 \end{bmatrix}$$

**Parameters:**

* **Kernel Size:** Fixed at 3x3.
* **Padding:** Zero-padding.

**Explicit Outputs & Meanings:**

1. **$X$ Gradient (Output 1):** Detects vertical lines. Compared to Sobel, these lines might appear slightly blockier or noisier if the original image has film grain.
2. **$Y$ Gradient (Output 2):** Detects horizontal lines.
3. **Magnitude (Final Output):** The combined edge map.

---

## 3. The Roberts Cross

**Concept:** Roberts uses a tiny 2x2 grid to calculate the differences along the diagonals rather than straight axes. Because the kernel is so small, it captures almost zero surrounding context, making it extremely sensitive to sharp transitions but highly vulnerable to noise.

**Mathematical Kernels:**


$$G_x = \begin{bmatrix} 1 & 0 \\ 0 & -1 \end{bmatrix} \quad G_y = \begin{bmatrix} 0 & 1 \\ -1 & 0 \end{bmatrix}$$


*(Note: In our C++ implementation, these are padded with zeros to fit our standard 3x3 convolution loop).*

**Parameters:**

* **Kernel Size:** Fixed at 2x2 (computed within a 3x3 window).
* **Padding:** Zero-padding.

**Explicit Outputs & Meanings:**

1. **$X$ Gradient (Output 1):** Detects edges slanting at a **+45 degree diagonal**.
2. **$Y$ Gradient (Output 2):** Detects edges slanting at a **-45 degree diagonal**.
3. **Magnitude (Final Output):** The combined edge map. The resulting lines are incredibly thin and sharp, but often fragmented if the image isn't perfectly crisp.

---

## 4. The Canny Edge Detector (OpenCV)

**Concept:** Unlike the previous three, Canny is a multi-stage algorithm that outputs perfect, 1-pixel-thin continuous edges. Per the assignment, X and Y previews are not required for this method.

**The Internal Pipeline (Intermediate Outputs):**

1. **Gaussian Blur:** The image is mathematically smoothed to destroy noise.
2. **Sobel Filtering:** The smoothed image is run through Sobel $X$ and $Y$ filters to find the raw Gradients and the Direction/Angle of the edges ($\theta$).
3. **Non-Maximum Suppression:** The algorithm walks along the angle of the edge. If a pixel isn't the absolute brightest point in that direction, it is turned black (suppressed). This thins thick edges down to 1 pixel.
4. **Hysteresis Tracking:** The algorithm uses the user-defined thresholds to finalize the edges.

**Explicit Parameters:**

* **Lower Threshold (e.g., 50.0):** Any pixel with a gradient magnitude below this number is immediately discarded as noise.
* **Upper Threshold (e.g., 150.0):** Any pixel with a gradient magnitude above this number is classified as a "Sure Edge" and kept permanently.
* *Hysteresis Logic:* Pixels that fall *between* the lower and upper thresholds are classified as "Weak Edges". They are only kept if they physically touch a "Sure Edge", ensuring continuous lines.

**Explicit Outputs & Meanings:**

* **Final Output:** A single binary map (pixels are strictly 0 or 255). It extracts precise structural contours ideal for object detection.


---

# Part 2: Code Implementation & Architecture

This section provides a comprehensive, line-by-line breakdown of the `EdgeDetectors` class. It traces exactly how a `cv::Mat` (OpenCV Matrix) travels from the user interface, through our custom preprocessing and convolution engines, and back to the screen.

## 1. How the Image is Received (The Routing)

The edge detection module does not interact directly with the user interface. Instead, it waits for a command from the `AppController`.

When a user selects an edge mask and clicks "Apply", the `AppController` grabs the current image from the `ImagePanel` and passes it directly into one of our public static functions.
For example: `cv::Mat result = EdgeDetectors::applySobel(inputImage);`

## 2. The Header File: `EdgeDetectors.h`

This file defines the blueprint of our module. We use a class with entirely `static` methods. This means the `AppController` doesn't need to create an "EdgeDetector object" to use it; it can just call the math functions directly like a utility library.

```cpp
#ifndef EDGEDETECTORS_H
#define EDGEDETECTORS_H

#include <opencv2/opencv.hpp>
#include <vector>

class EdgeDetectors {
public:
    // --- PUBLIC API ---
    // These are the entry points called by the AppController. 
    // They receive the raw uploaded image and return the final processed image.
    static cv::Mat applySobel(const cv::Mat& input);
    static cv::Mat applyPrewitt(const cv::Mat& input);
    static cv::Mat applyRoberts(const cv::Mat& input);
    static cv::Mat applyCanny(const cv::Mat& input, double lowerThresh, double upperThresh);

private:
    // --- INTERNAL ENGINE ---
    // These functions do the heavy lifting but are hidden from the rest of the app.
    
    // The manual 2D convolution engine that bypasses cv::filter2D
    static cv::Mat convolve(const cv::Mat& input, const std::vector<std::vector<int>>& kernel);
    
    // Calculates magnitude and glues the X, Y, and Mag images together horizontally
    static cv::Mat generatePreview(const cv::Mat& gradX, const cv::Mat& gradY);
};

#endif // EDGEDETECTORS_H

```

---

## 3. The Implementation File: `EdgeDetectors.cpp`

### A. The Core Math Engine: `convolve()`

This function is the heart of the manual edge detectors. It takes a grayscale image and a 2D vector (the kernel) and applies the sliding window math.

```cpp
cv::Mat EdgeDetectors::convolve(const cv::Mat& input, const std::vector<std::vector<int>>& kernel) {
    // 1. DYNAMIC KERNEL SIZING
    int kRows = kernel.size();       // Typically 3
    int kCols = kernel[0].size();    // Typically 3
    
    // Calculate the padding needed to center the kernel over a pixel.
    // For a 3x3 kernel, 3/2 = 1. So we look 1 pixel up/down/left/right.
    int padY = kRows / 2; 
    int padX = kCols / 2;

    // 2. ALLOCATE MEMORY (CRITICAL STEP)
    // We create a blank matrix of the same size as the input.
    // We MUST use CV_32F (32-bit float). Standard images are CV_8U (0-255). 
    // Gradient math produces negative numbers (e.g., -400). If we use CV_8U, 
    // OpenCV will clip those to 0, permanently destroying half the edge data!
    cv::Mat output = cv::Mat::zeros(input.size(), CV_32F);

    // 3. THE SLIDING WINDOW
    // Outer loops: Iterate through every pixel in the image.
    // Notice we start at 'padY' and end at 'input.rows - padY'. 
    // We intentionally ignore the outermost 1-pixel border of the image 
    // so our kernel doesn't reach outside the image memory and crash the app.
    for (int y = padY; y < input.rows - padY; ++y) {
        for (int x = padX; x < input.cols - padX; ++x) {
            
            float sum = 0.0f; // Reset the math for the new center pixel
            
            // Inner loops: Iterate through the 3x3 kernel grid
            for (int ky = -padY; ky <= padY; ++ky) {
                for (int kx = -padX; kx <= padX; ++kx) {
                    
                    // Grab the actual image pixel under this part of the kernel
                    // .at<uchar> is used because the input image is an 8-bit unsigned char
                    int pixel = input.at<uchar>(y + ky, x + kx);
                    
                    // Grab the corresponding weight from our mathematical kernel
                    int weight = kernel[ky + padY][kx + padX];
                    
                    // Multiply and accumulate
                    sum += pixel * weight;
                }
            }
            // Store the final calculated float value in our output matrix
            output.at<float>(y, x) = sum;
        }
    }
    return output;
}

```

### B. The Preview Generator: `generatePreview()`

After `convolve()` runs twice (once for X, once for Y), this function takes those two floating-point matrices, calculates the final edge strength, and formats them for the UI.

```cpp
cv::Mat EdgeDetectors::generatePreview(const cv::Mat& gradX, const cv::Mat& gradY) {
    // Create a blank floating-point matrix for the Pythagorean math
    cv::Mat magnitude = cv::Mat::zeros(gradX.size(), CV_32F);

    // 1. CALCULATE MAGNITUDE
    // Loop through every pixel and apply: G = sqrt(Gx^2 + Gy^2)
    for (int y = 0; y < gradX.rows; ++y) {
        for (int x = 0; x < gradX.cols; ++x) {
            float gx = gradX.at<float>(y, x);
            float gy = gradY.at<float>(y, x);
            magnitude.at<float>(y, x) = std::sqrt(gx*gx + gy*gy);
        }
    }

    // 2. NORMALIZE FOR DISPLAY
    // We currently have floats ranging wildly from -1000 to +1000.
    // Qt and OpenCV cannot display this on a screen.
    // cv::convertScaleAbs takes the absolute value (turning negatives positive) 
    // and strictly scales the numbers back into a viewable 0 to 255 range.
    cv::Mat dispX, dispY, dispMag;
    cv::convertScaleAbs(gradX, dispX); 
    cv::convertScaleAbs(gradY, dispY);
    cv::convertScaleAbs(magnitude, dispMag);

    // 3. GLUE TOGETHER (hconcat)
    // Put the three 8-bit matrices into a list and use horizontal concatenation
    // to stick them side-by-side into one super-wide image for the UI to display.
    cv::Mat preview;
    std::vector<cv::Mat> matrices = {dispX, dispY, dispMag};
    cv::hconcat(matrices, preview);
    
    return preview;
}

```

### C. The Filter Application Methods

These are the public methods that define the specific kernels and push the image through the pipeline. (Using Sobel as the primary example, as Prewitt and Roberts follow the exact same structural logic).

```cpp
cv::Mat EdgeDetectors::applySobel(const cv::Mat& input) {
    // 1. PREPROCESSING
    // Never alter the original input pointer. Clone it first.
    cv::Mat gray = input.clone();
    
    // Edge detection requires a 1-channel grayscale image.
    // If the image has 3 channels (BGR), convert it.
    if (gray.channels() == 3) {
        cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
    }

    // 2. DEFINE THE MASKS
    // These are standard C++ 2D vectors defining the math for Sobel
    std::vector<std::vector<int>> sobelX = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    std::vector<std::vector<int>> sobelY = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // 3. EXECUTE CONVOLUTION
    // Pass the grayscale image and the masks into our engine
    cv::Mat gradX = convolve(gray, sobelX);
    cv::Mat gradY = convolve(gray, sobelY);

    // 4. RETURN FORMATTED OUTPUT
    return generatePreview(gradX, gradY);
}

```

### D. The Canny Exception

Because Canny is a multi-stage algorithm rather than a single kernel pass, it bypasses our `convolve()` and `generatePreview()` functions entirely.

```cpp
cv::Mat EdgeDetectors::applyCanny(const cv::Mat& input, double lowerThresh, double upperThresh) {
    // Preprocessing: Convert to Grayscale
    cv::Mat gray = input.clone();
    if (gray.channels() == 3) cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    // Allocate memory for the final binary output map
    cv::Mat edges;
    
    // Execute the OpenCV library function using the parameters passed from the UI
    cv::Canny(gray, edges, lowerThresh, upperThresh);
    
    // Return the single binary image immediately
    return edges; 
}

```

---

## 🛠️ How to Modify or Extend This Code

Because of the highly modular architecture, adding a brand new manual edge detector (like the Scharr operator or the Laplacian) takes less than a minute.

1. **Add the declaration** in `EdgeDetectors.h` (e.g., `static cv::Mat applyScharr(const cv::Mat& input);`).
2. **Copy and paste** the `applySobel` function structure in the `.cpp` file.
3. **Change the values** inside the `std::vector<std::vector<int>>` variable to match your new mathematical kernel.
4. **Wire it to the UI** by adding a new `else if` string check in `AppController.cpp`.

The `convolve()` engine is dynamically sized by `kernel.size()`, meaning you could pass it a 5x5 or 7x7 custom mask and it will automatically adjust its padding and mathematics without requiring any changes to the core engine code.