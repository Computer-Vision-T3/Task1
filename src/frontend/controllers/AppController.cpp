#include "AppController.h"
#include "../MainWindow.h"
#include "../components/TopTaskBar.h"
#include "../components/ImagePanel.h"
#include "../../backend/Module2_EdgesAndEntropy/EdgeDetectors.h"
#include "../../backend/Module2_EdgesAndEntropy/EntropyCalculator.h" 
#include "../../backend/Module5_FrequencyAndHybrid/HybridImageBuilder.h"
#include "../../backend/Module5_FrequencyAndHybrid/FrequencyFilters.h" 
#include "../../backend/Module1_NoiseAndFilters/NoiseGenerator.h"
#include "../../backend/Module1_NoiseAndFilters/LowPassFilters.h"
// Updated paths and includes for your current backend setup
#include "../../backend/Module3_HistogramsAndColor/HistogramTools.h"
#include "../../backend/Module3_HistogramsAndColor/ColorTransformations.h"
#include <QInputDialog> 
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>

namespace {
cv::Mat toGray(const cv::Mat& image) {
    if (image.empty()) return image;
    if (image.channels() == 1) return image;

    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    } else {
        gray = image.clone();
    }
    return gray;
}

bool saveOutputImage(const cv::Mat& image, int taskIndex) {
    if (image.empty()) return false;

    QDir outputDir(QCoreApplication::applicationDirPath());
    outputDir.cdUp(); // Move from build/ to project root
    if (!outputDir.exists("test")) {
        outputDir.mkpath("test");
    }

    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
    const QString fileName = QString("task%1_%2.png").arg(taskIndex).arg(timestamp);
    const QString filePath = outputDir.filePath("test/" + fileName);
    return cv::imwrite(filePath.toStdString(), image);
}
}

AppController::AppController(MainWindow* window, QObject *parent)
    : QObject(parent), mainWindow(window) {
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::taskChanged, this, &AppController::handleTaskChange);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::applyRequested, this, &AppController::handleApply);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::saveRequested, this, &AppController::handleSave);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::clearRequested, this, &AppController::handleClear);
}

void AppController::handleTaskChange(int taskIndex) {
    mainWindow->updateLayoutForTask(taskIndex);
}

void AppController::handleApply() {
    int taskIndex = mainWindow->getTopTaskBar()->getSelectedOperation();
    auto& inputs = mainWindow->getInputPanels();
    auto& outputs = mainWindow->getOutputPanels();

    if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;
    cv::Mat originalImg = inputs[0]->getImage();
    cv::Mat currentImg = toGray(originalImg);

    auto displayOutput = [&](const cv::Mat& image) {
        if (image.empty()) return;
        cv::Mat grayImage = toGray(image);
        if (!outputs.isEmpty()) outputs[0]->displayImage(grayImage);
    };
        // --- TASK 1: ADD NOISE ---
    if (taskIndex == 1) {
        ParameterBox* paramBox = mainWindow->getTopTaskBar()->getParameterBox();
        QComboBox* noiseTypeCombo = paramBox->findChild<QComboBox*>("noiseTypeCombo");
        QSlider* intensitySlider = paramBox->findChild<QSlider*>("noiseIntensitySlider");

        if (!noiseTypeCombo || !intensitySlider) return; // safety check

        QString noiseType = noiseTypeCombo->currentText();
        int intensity = intensitySlider->value(); // 0 - 100
        cv::Mat result;
        if (noiseType == "Uniform") {
            result = NoiseGenerator::addUniformNoise(currentImg, intensity);
        } 
        else if (noiseType == "Gaussian") {
            result = NoiseGenerator::addGaussianNoise(currentImg, intensity);
        } 
        else if (noiseType == "Salt & Pepper") {
            double amount = intensity / 100.0; // slider من 0 لـ 100 يتحول لنسبة
            result = NoiseGenerator::addSaltPepperNoise(currentImg, amount);
        }
        displayOutput(result);
    }
    // --- TASK 2: LOW PASS FILTER ---
    else if (taskIndex == 2) {
        ParameterBox* paramBox = mainWindow->getTopTaskBar()->getParameterBox();
        QComboBox* filterTypeCombo = paramBox->findChild<QComboBox*>("filterTypeCombo");
        QSpinBox* kernelSizeSpin = paramBox->findChild<QSpinBox*>("kernelSizeSpin");

        if (!filterTypeCombo || !kernelSizeSpin) return; // safety check

        QString filterType = filterTypeCombo->currentText();
        int kernelSize = kernelSizeSpin->value();
        cv::Mat result;
        if (filterType == "Average") {
            result = LowPassFilters::applyAverage(currentImg, kernelSize);
        } 
        else if (filterType == "Gaussian") {
            result = LowPassFilters::applyGaussian(currentImg, kernelSize);
        } 
        else if (filterType == "Median") {
            result = LowPassFilters::applyMedian(currentImg, kernelSize);
        }
        displayOutput(result);
    }
    // --- TASK 3: EDGE DETECTION ---
    if (taskIndex == 3) {
        QStringList items;
        items << "Sobel (Manual)" << "Prewitt (Manual)" << "Roberts (Manual)" << "Canny (OpenCV)";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Edge Detection", "Select Edge Mask:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            cv::Mat result;
            if (item == "Sobel (Manual)") result = EdgeDetectors::applySobel(currentImg);
            else if (item == "Prewitt (Manual)") result = EdgeDetectors::applyPrewitt(currentImg);
            else if (item == "Roberts (Manual)") result = EdgeDetectors::applyRoberts(currentImg);
            else if (item == "Canny (OpenCV)") result = EdgeDetectors::applyCanny(currentImg, 50.0, 150.0);
            
            displayOutput(result);
        }
    }

    // --- TASK 4: DRAW HISTOGRAM AND DISTRIBUTION CURVE ---
    else if (taskIndex == 4) {
        cv::Mat grayImg;
        // Ensure image is grayscale for Task 4
        if (currentImg.channels() > 1) {
            grayImg = ColorTransformations::convertToGray(currentImg);
        } else {
            grayImg = currentImg;
        }

        cv::Mat hist, cdf;
        // Use exact function name from backend
        HistogramTools::getHistogramAndCDF(grayImg, hist, cdf);
        // Use exact function name for visualization
        cv::Mat plot = HistogramTools::plotHistogram(hist, cdf, cv::Scalar(255, 255, 255));
        
        displayOutput(plot);
    }

    // --- TASK 7: ENTROPY ---
    else if (taskIndex == 7) {
        double entropy = EntropyCalculator::calculate(currentImg);
        cv::Mat histGraph = EntropyCalculator::plotHistogram(currentImg);
        displayOutput(histGraph);

        QString explanation = QString("<h3>Entropy Analysis</h3><hr><p><b>Value:</b> %1</p>").arg(entropy, 0, 'f', 4);
        mainWindow->getInfoSidebar()->setHtml(explanation);
    }

    // --- TASK 8: COLOR TO GRAY & RGB HISTOGRAMS ---
    else if (taskIndex == 8) {
        // 1. Transform and display Grayscale image
        cv::Mat grayResult = toGray(currentImg);
        displayOutput(grayResult);

        // 2. Open pop-up windows for R, G, and B distributions
        ColorTransformations::analyzeRGB(originalImg);
    }

    // --- TASK 9: FREQUENCY FILTERS ---
    else if (taskIndex == 9) {
        QStringList items;
        items << "Low Pass" << "High Pass";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Filter Choice", "Select Type:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            FrequencyFilters::FilterType type = (item == "Low Pass") ? FrequencyFilters::LOW_PASS : FrequencyFilters::HIGH_PASS;
            cv::Mat result = FrequencyFilters::applyFilter(currentImg, 50.0f, type);
            displayOutput(result);
        }
    }

    // --- TASK 10: HYBRID IMAGES ---
    else if (taskIndex == 10) {
        if (inputs.size() >= 2) {
            cv::Mat img2 = toGray(inputs[1]->getImage());
            if (!currentImg.empty() && !img2.empty()) {
                cv::Mat result = HybridImageBuilder::createHybrid(currentImg, img2, 15);
                displayOutput(result);
            }
        }
    }
}

void AppController::handleSave() {
    int taskIndex = mainWindow->getTopTaskBar()->getSelectedOperation();
    auto& outputs = mainWindow->getOutputPanels();
    if (outputs.isEmpty()) return;

    cv::Mat outputImage = outputs[0]->getImage();
    if (outputImage.empty()) return;

    saveOutputImage(outputImage, taskIndex);
}

void AppController::handleClear() {
    for(auto* panel : mainWindow->getOutputPanels()) {
        panel->clear();
    }
}