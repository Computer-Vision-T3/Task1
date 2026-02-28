#include "ParameterBox.h"

ParameterBox::ParameterBox(QWidget *parent) : QWidget(parent) {
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 
    layout->setSpacing(8);
}

void ParameterBox::clearLayout() {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}

void ParameterBox::updateParametersForTask(int taskIndex) {
    clearLayout();

    switch (taskIndex) {
        case 0: { // 1. Add Noise
            layout->addWidget(new QLabel("Noise Type:", this));
            QComboBox* noiseType = new QComboBox(this);
            noiseType->setObjectName("noiseTypeCombo");
            noiseType->addItems({"Uniform", "Gaussian", "Salt & Pepper"});
            layout->addWidget(noiseType);

            QLabel* localBubble = new QLabel("?", this);
            localBubble->setObjectName("helpBubble");
            layout->addWidget(localBubble);

            auto updateNoiseHelp = [localBubble](const QString& text) {
                if (text == "Uniform") localBubble->setToolTip("Applies random static evenly across the image.");
                else if (text == "Gaussian") localBubble->setToolTip("Applies bell-curve noise, simulating low-light camera ISO static.");
                else if (text == "Salt & Pepper") localBubble->setToolTip("Randomly turns pixels pure white or pure black (data corruption).");
            };
            connect(noiseType, &QComboBox::currentTextChanged, updateNoiseHelp);
            updateNoiseHelp(noiseType->currentText());

            layout->addWidget(new QLabel("Intensity:", this));
            QSlider* intensitySlider = new QSlider(Qt::Horizontal, this);
            intensitySlider->setRange(0, 100);
            intensitySlider->setObjectName("noiseIntensitySlider");
            intensitySlider->setValue(20);
            layout->addWidget(intensitySlider);
            break;
        }
        case 1: { // 2. Low Pass Filter
            layout->addWidget(new QLabel("Filter:", this));
            QComboBox* filterType = new QComboBox(this);
            filterType->setObjectName("filterTypeCombo");
            filterType->addItems({"Average", "Gaussian", "Median"});
            layout->addWidget(filterType);

            QLabel* localBubble = new QLabel("?", this);
            localBubble->setObjectName("helpBubble");
            layout->addWidget(localBubble);

            auto updateFilterHelp = [localBubble](const QString& text) {
                if (text == "Average") localBubble->setToolTip("A simple blur. Replaces each pixel with the mean of its neighbors.");
                else if (text == "Gaussian") localBubble->setToolTip("A weighted blur that keeps the center more in focus. Preserves edges better.");
                else if (text == "Median") localBubble->setToolTip("Replaces the pixel with the median. The ultimate destroyer of Salt & Pepper noise!");
            };
            connect(filterType, &QComboBox::currentTextChanged, updateFilterHelp);
            updateFilterHelp(filterType->currentText());

            layout->addWidget(new QLabel("Kernel Size:", this));
            QSpinBox* kernelSize = new QSpinBox(this);
            kernelSize->setObjectName("kernelSizeSpin");
            kernelSize->setRange(3, 31);
            kernelSize->setSingleStep(2); 
            kernelSize->setValue(3); 
            layout->addWidget(kernelSize);
            break;
        }
        case 2: { // 3. Edge Detection
            layout->addWidget(new QLabel("Mask:", this));
            QComboBox* maskType = new QComboBox(this);
            maskType->setObjectName("maskTypeCombo");
            maskType->addItems({"Sobel", "Roberts", "Prewitt", "Canny"});
            layout->addWidget(maskType);

            QLabel* localBubble = new QLabel("?", this);
            localBubble->setObjectName("helpBubble");
            layout->addWidget(localBubble);

            auto updateEdgeHelp = [localBubble](const QString& text) {
                if (text == "Sobel") localBubble->setToolTip("Standard 3x3 mask. Highlights thick edges and ignores light noise.");
                else if (text == "Roberts") localBubble->setToolTip("Tiny 2x2 diagonal mask. Finds incredibly thin edges but is highly vulnerable to noise.");
                else if (text == "Prewitt") localBubble->setToolTip("Similar to Sobel, but doesn't place extra weight on the center pixel.");
                else if (text == "Canny") localBubble->setToolTip("The industry standard. Multi-stage algorithm that outputs perfect, 1-pixel thin continuous lines.");
            };
            connect(maskType, &QComboBox::currentTextChanged, updateEdgeHelp);
            updateEdgeHelp(maskType->currentText());
            break;
        }
        case 5: { // 6. Equalization
            layout->addWidget(new QLabel("Mode:", this));
            QComboBox* eqMode = new QComboBox(this);
            eqMode->setObjectName("eqModeCombo");
            eqMode->addItems({"Grayscale Equalization", "RGB Equalization"});
            layout->addWidget(eqMode);

            QLabel* localBubble = new QLabel("?", this);
            localBubble->setObjectName("helpBubble");
            layout->addWidget(localBubble);

            auto updateEqHelp = [localBubble](const QString& text) {
                if (text == "Grayscale Equalization") localBubble->setToolTip("Converts to black & white, then perfectly distributes the contrast.");
                else if (text == "RGB Equalization") localBubble->setToolTip("Calculates Cumulative Distribution for Red, Green, and Blue separately. Can cause crazy color shifts!");
            };
            connect(eqMode, &QComboBox::currentTextChanged, updateEqHelp);
            updateEqHelp(eqMode->currentText());
            break;
        }
        case 8: { // 9. Frequency Filters
            layout->addWidget(new QLabel("Filter Type:", this));
            QComboBox* freqType = new QComboBox(this);
            freqType->setObjectName("freqTypeCombo");
            freqType->addItems({"Low Pass", "High Pass"});
            layout->addWidget(freqType);

            QLabel* localBubble = new QLabel("?", this);
            localBubble->setObjectName("helpBubble");
            layout->addWidget(localBubble);

            auto updateFreqHelp = [localBubble](const QString& text) {
                if (text == "Low Pass") localBubble->setToolTip("Blocks high frequencies (sharp edges). The result is a blurry image.");
                else if (text == "High Pass") localBubble->setToolTip("Blocks low frequencies (flat colors). The result is a black canvas with sharp outlines.");
            };
            connect(freqType, &QComboBox::currentTextChanged, updateFreqHelp);
            updateFreqHelp(freqType->currentText());
            break;
        }
        default: {
            layout->addWidget(new QLabel("Ready to Apply.", this));
            break;
        }
    }
}