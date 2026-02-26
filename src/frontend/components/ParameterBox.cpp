#include "ParameterBox.h"

ParameterBox::ParameterBox(QWidget *parent) : QWidget(parent) {
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Keep it flush with the taskbar
}

void ParameterBox::clearLayout() {
    QLayoutItem* item;
    // Loop through and delete all widgets currently in the parameter box
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void ParameterBox::updateParametersForTask(int taskIndex) {
    clearLayout();

    // Dynamically inject the correct UI controls based on the selected task
    switch (taskIndex) {
        case 0: { // 1. Add Noise
            layout->addWidget(new QLabel("Noise Type:", this));
            QComboBox* noiseType = new QComboBox(this);
            noiseType->addItems({"Uniform", "Gaussian", "Salt & Pepper"});
            layout->addWidget(noiseType);
            break;
        }
        case 1: { // 2. Low Pass Filter
            layout->addWidget(new QLabel("Filter:", this));
            QComboBox* filterType = new QComboBox(this);
            filterType->addItems({"Average", "Gaussian", "Median"});
            layout->addWidget(filterType);

            layout->addWidget(new QLabel("Kernel Size:", this));
            QSpinBox* kernelSize = new QSpinBox(this);
            kernelSize->setRange(3, 31);
            kernelSize->setSingleStep(2); // Kernels are usually odd numbers (3, 5, 7...)
            layout->addWidget(kernelSize);
            break;
        }
        case 2: { // 3. Edge Detection
            layout->addWidget(new QLabel("Mask:", this));
            QComboBox* maskType = new QComboBox(this);
            maskType->addItems({"Sobel", "Roberts", "Prewitt", "Canny"});
            layout->addWidget(maskType);
            break;
        }
        // You can add more cases here for the other 7 tasks later!
        default: {
            layout->addWidget(new QLabel("No parameters needed.", this));
            break;
        }
    }
}