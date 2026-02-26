Here is the content formatted as a clean, professional `.md` (Markdown) file.

```markdown
# Image Processing Module: Noise & Filtering

This module provides a comprehensive environment for adding artificial noise to images and applying low-pass filters to smooth them. It is designed for testing denoising algorithms, evaluating robustness, and understanding image degradation.

---

## Part 1: Noise Addition & Low Pass Filtering

### Task 1: Add Noise
**Description:** Introduces artificial noise to simulate real-world image imperfections.

#### Inputs
* **Input Image:** Single source image.
* **Adjustable Parameters:**
    * **Noise Type:** * *Uniform:* Evenly distributed random noise.
        * *Gaussian:* Normally distributed noise (bell curve) around pixel values.
        * *Salt & Pepper:* Randomly replaces pixels with black (0) or white (255) dots.
    * **Intensity (0–100):** * Controls the strength/visibility of the noise.
        * For Salt & Pepper, this represents the fraction of affected pixels.

#### Mathematical Pipeline
1.  **Uniform Noise:** $I_{noisy} = I + U(-\text{intensity}, +\text{intensity})$  
    Pixels are incremented by a value sampled from a uniform distribution.
    
2.  **Gaussian Noise:** $I_{noisy} = I + N(0, \sigma^2), \quad \sigma = \text{intensity}$  
    Pixels are incremented by a value with mean 0 and standard deviation $\sigma$.

3.  **Salt & Pepper Noise:** Randomly selects a fraction of pixels (based on $\frac{\text{intensity}}{100}$) and forces them to $0$ or $255$.



---

### Task 2: Low Pass Filter
**Description:** Applies smoothing filters to reduce noise or unwanted high-frequency details.

#### Inputs
* **Input Image:** Noisy or original image.
* **Adjustable Parameters:**
    * **Filter Type:**
        * *Average (Box Filter):* Mean of surrounding pixels.
        * *Gaussian:* Weighted average using a Gaussian kernel; preserves edges better than Average.
        * *Median:* Replaces pixel with the median of neighbors; highly effective against Salt & Pepper noise.
    * **Kernel Size (3–31):** * Must be an **odd integer**.
        * Larger kernels result in stronger smoothing but more blur.

#### Mathematical Pipeline
1.  **Average Filter:** $I_{smooth}(x, y) = \frac{1}{k^2} \sum_{i=-r}^{r} \sum_{j=-r}^{r} I(x+i, y+j)$  
    Where $k$ is kernel size and $r = \frac{k-1}{2}$.

2.  **Gaussian Filter:** $I_{smooth}(x, y) = \sum_{i=-r}^{r} \sum_{j=-r}^{r} I(x+i, y+j) \cdot G(i,j,\sigma)$  
    Uses a weighted kernel $G$ where central pixels have higher influence.

3.  **Median Filter:** Sorts all pixels in the $k \times k$ neighborhood and selects the middle value.



---

## Part 2: Code Implementation & Architecture

### 1. Routing via AppController
The `AppController` acts as the orchestrator, listening for the **Apply** button trigger. It retrieves the image and parameters before routing to the specific logic:

```cpp
if (taskIndex == 1) applyNoise(currentImage);
else if (taskIndex == 2) applyLowPassFilter(currentImage);

```

### 2. ParameterBox Integration

* **Noise Task:** Uses `noiseTypeCombo` (QComboBox) and `noiseIntensitySlider` (QSlider).
* **Low Pass Task:** Uses `filterTypeCombo` (QComboBox) and `kernelSizeSpin` (QSpinBox).

### 3. Processing Functions

* **NoiseGenerator:** Contains static methods like `addUniformNoise` and `addSaltPepperNoise` which return a processed `cv::Mat`.
* **LowPassFilters:** Contains logic for `applyAverage`, `applyGaussian`, and `applyMedian`.

### 4. Output Display

Once the `cv::Mat result` is calculated, it is pushed to the UI:

```cpp
if (!outputs.isEmpty()) {
    outputs[0]->displayImage(result);
}

```

> **Note:** All filters implement zero-padding at image borders to prevent memory access violations during kernel convolution.
