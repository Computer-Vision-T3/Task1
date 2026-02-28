#include "AppController.h"
#include "../MainWindow.h"
#include "../components/TopTaskBar.h"
#include "../components/ImagePanel.h"
#include "../../backend/Module1_NoiseAndFilters/NoiseGenerator.h"
#include "../../backend/Module1_NoiseAndFilters/LowPassFilters.h"
#include "../../backend/Module2_EdgesAndEntropy/EdgeDetectors.h"
#include "../../backend/Module2_EdgesAndEntropy/EntropyCalculator.h" 
#include "../../backend/Module3_HistogramsAndColor/HistogramTools.h"
#include "../../backend/Module3_HistogramsAndColor/ColorTransformations.h"
#include "../../backend/Module4_Enhancement/ImageEqualizer.h"
#include "../../backend/Module4_Enhancement/ImageNormalizer.h"
#include "../../backend/Module5_FrequencyAndHybrid/HybridImageBuilder.h"
#include "../../backend/Module5_FrequencyAndHybrid/FrequencyFilters.h" 

#include <QFileDialog>  
#include <QMessageBox>  
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>

AppController::AppController(MainWindow* window, QObject *parent)
    : QObject(parent), mainWindow(window) {
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::taskChanged, this, &AppController::handleTaskChange);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::applyRequested, this, &AppController::handleApply);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::clearRequested, this, &AppController::handleClear);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::saveRequested, this, &AppController::handleSave);
}

void AppController::handleTaskChange(int taskIndex) {
    mainWindow->updateLayoutForTask(taskIndex);
}

void AppController::handleApply() {
    int taskIndex = mainWindow->getTopTaskBar()->getSelectedOperation();
    auto& inputs = mainWindow->getInputPanels();
    auto& outputs = mainWindow->getOutputPanels();

    if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;
    cv::Mat currentImg = inputs[0]->getImage();

    ParameterBox* pBox = mainWindow->getTopTaskBar()->getParameterBox();

    // --- TASK 1: ADD NOISE ---
    if (taskIndex == 1) {
        QComboBox* combo = pBox->findChild<QComboBox*>("noiseTypeCombo");
        QSlider* slider = pBox->findChild<QSlider*>("noiseIntensitySlider");
        if (!combo || !slider) return;

        QString item = combo->currentText();
        int intensity = slider->value();
        cv::Mat result;

        if (item == "Uniform") result = NoiseGenerator::addUniformNoise(currentImg, intensity); 
        else if (item == "Gaussian") result = NoiseGenerator::addGaussianNoise(currentImg, intensity / 2.0); 
        else if (item == "Salt & Pepper") result = NoiseGenerator::addSaltPepperNoise(currentImg, intensity / 100.0); 

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // --- TASK 2: LOW PASS FILTERS ---
    else if (taskIndex == 2) {
        QComboBox* combo = pBox->findChild<QComboBox*>("filterTypeCombo");
        QSpinBox* spin = pBox->findChild<QSpinBox*>("kernelSizeSpin");
        if (!combo || !spin) return;

        QString item = combo->currentText();
        int kernelSize = spin->value();
        cv::Mat result;

        if (item == "Average") result = LowPassFilters::applyAverage(currentImg, kernelSize); 
        else if (item == "Gaussian") result = LowPassFilters::applyGaussian(currentImg, kernelSize);
        else if (item == "Median") result = LowPassFilters::applyMedian(currentImg, kernelSize);

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // --- TASK 3: EDGE DETECTION ---
    else if (taskIndex == 3) {
        QComboBox* combo = pBox->findChild<QComboBox*>("maskTypeCombo");
        if(!combo) return;
        QString item = combo->currentText();

        if (item == "Canny") {
            cv::Mat result = EdgeDetectors::applyCanny(currentImg, 50.0, 150.0);
            if (outputs.size() >= 3) {
                outputs[0]->clear(); outputs[1]->clear(); 
                outputs[2]->displayImage(result);         
            }
        } else {
            std::vector<cv::Mat> results;
            if (item == "Sobel") results = EdgeDetectors::applySobel(currentImg);
            else if (item == "Prewitt") results = EdgeDetectors::applyPrewitt(currentImg);
            else if (item == "Roberts") results = EdgeDetectors::applyRoberts(currentImg);
            
            if (outputs.size() >= 3 && results.size() == 3) {
                outputs[0]->displayImage(results[0]); 
                outputs[1]->displayImage(results[1]); 
                outputs[2]->displayImage(results[2]); 
            }
        }
    }

    // --- TASK 4: DRAW HISTOGRAM AND DISTRIBUTION CURVE ---
    else if (taskIndex == 4) {
        cv::Mat grayImg;
        if (currentImg.channels() > 1) {
            grayImg = ColorTransformations::convertToGray(currentImg);
        } else {
            grayImg = currentImg;
        }

        cv::Mat hist, cdf;
        HistogramTools::getHistogramAndCDF(grayImg, hist, cdf);
        // Using Neon Cyan for the single histogram (BGR format: 255 Blue, 200 Green, 0 Red)
        cv::Mat plot = HistogramTools::plotHistogram(hist, cdf, cv::Scalar(255, 200, 0));
        
        if (!outputs.isEmpty()) outputs[0]->displayImage(plot);
    }

    // --- TASK 5: NORMALIZE IMAGE ---
    else if (taskIndex == 5) {
        ImageNormalizer normalizer;
        cv::Mat normalizedFloat = normalizer.normalize_image(currentImg);
        cv::Mat normalizedDisplay;
        normalizedFloat.convertTo(normalizedDisplay, CV_8U, 255.0);
        if (!outputs.isEmpty()) outputs[0]->displayImage(normalizedDisplay);
    }

    // --- TASK 6: EQUALIZE IMAGE ---
    else if (taskIndex == 6) {
        QComboBox* combo = pBox->findChild<QComboBox*>("eqModeCombo");
        if(!combo) return;
        
        ImageEqualizer equalizer;
        cv::Mat result;

        if (combo->currentText() == "Grayscale Equalization") {
            cv::Mat grayImg;
            if (currentImg.channels() > 1) cv::cvtColor(currentImg, grayImg, cv::COLOR_BGR2GRAY);
            else grayImg = currentImg;
            result = equalizer.equalize_grayScale(grayImg);
        } else {
            if (currentImg.channels() == 3) result = equalizer.equalize_rgb(currentImg);
            else result = equalizer.equalize_grayScale(currentImg);
        }

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // --- TASK 7: ENTROPY ---
    else if (taskIndex == 7) {
        double entropy = EntropyCalculator::calculate(currentImg);
        cv::Mat histGraph = EntropyCalculator::plotHistogram(currentImg);
        if (!outputs.isEmpty()) outputs[0]->displayImage(histGraph);

        // Analyze the result for the user
        QString entropyLevel;
        if (entropy < 4.5) {
            entropyLevel = "<span style='color:#d63031;'><b>Low Complexity:</b></span> This image consists of large, uniform areas (like a clear sky or solid background). It lacks structural detail.";
        } else if (entropy < 6.8) {
            entropyLevel = "<span style='color:#0984e3;'><b>Moderate Complexity:</b></span> This is a standard photograph with a healthy natural variance of lighting and objects.";
        } else {
            entropyLevel = "<span style='color:#00b894;'><b>High Complexity:</b></span> This image is highly detailed, heavily textured, or contains intense high-frequency noise.";
        }

        QString explanation = QString(
            "<h3>Entropy Analysis</h3><hr>"
            "<p style='font-size: 16px;'><b>Calculated Value: %1</b></p>"
            "<p>%2</p>"
            "<hr>"
            "<p style='color:#636e72; font-size:11px;'><i>* Shannon Entropy mathematically measures the unpredictability of pixel intensities. Look at the graph: a wide, flat curve means high entropy. A spiky, narrow curve means low entropy.</i></p>"
        ).arg(entropy, 0, 'f', 4).arg(entropyLevel);
        
        mainWindow->getInfoSidebar()->setHtml(explanation);
    }

    // --- TASK 8: COLOR TO GRAY & RGB HISTOGRAMS ---
    else if (taskIndex == 8) {
        cv::Mat grayResult = ColorTransformations::convertToGray(currentImg);
        std::vector<cv::Mat> rgbPlots = ColorTransformations::analyzeRGB(currentImg);
        
        if (outputs.size() >= 4 && rgbPlots.size() == 3) {
            outputs[0]->displayImage(grayResult);
            outputs[1]->displayImage(rgbPlots[0]);
            outputs[2]->displayImage(rgbPlots[1]);
            outputs[3]->displayImage(rgbPlots[2]);
        }
    }

    // --- TASK 9: FREQUENCY FILTERS ---
    else if (taskIndex == 9) {
        QComboBox* combo = pBox->findChild<QComboBox*>("freqTypeCombo");
        if(!combo) return;
        
        FrequencyFilters::FilterType type = (combo->currentText() == "Low Pass") ? FrequencyFilters::LOW_PASS : FrequencyFilters::HIGH_PASS;
        cv::Mat result = FrequencyFilters::applyFilter(currentImg, 50.0f, type);
        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // --- TASK 10: HYBRID IMAGES ---
    else if (taskIndex == 10) {
        if (inputs.size() >= 2) {
            cv::Mat img2 = inputs[1]->getImage();
            if (!currentImg.empty() && !img2.empty()) {
                cv::Mat result = HybridImageBuilder::createHybrid(currentImg, img2, 15);
                if (!outputs.isEmpty()) outputs[0]->displayImage(result);
            }
        }
    }
}

void AppController::handleClear() {
    for(auto* panel : mainWindow->getOutputPanels()) {
        panel->clear();
    }
}

void AppController::handleSave() {
    auto& outputs = mainWindow->getOutputPanels();

    if (outputs.isEmpty() || outputs[0]->getImage().empty()) {
        QMessageBox::warning(mainWindow, "Save Error", "There is no processed image to save! Please apply a filter first.");
        return;
    }

    QString filter = "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)";
    QString fileName = QFileDialog::getSaveFileName(mainWindow, "Save Processed Image", "", filter);

    if (!fileName.isEmpty()) {
        cv::Mat imgToSave = outputs[0]->getImage();
        bool success = cv::imwrite(fileName.toStdString(), imgToSave);

        if (success) {
            QMessageBox::information(mainWindow, "Success", "Image saved successfully!");
        } else {
            QMessageBox::critical(mainWindow, "Error", "Failed to save the image. Please check the file path and extension.");
        }
    }
}