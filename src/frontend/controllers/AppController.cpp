#include "AppController.h"
#include "../MainWindow.h"
#include "../components/TopTaskBar.h"
#include "../components/ImagePanel.h"
#include "../../backend/Module2_EdgesAndEntropy/EdgeDetectors.h"
#include "../../backend/Module2_EdgesAndEntropy/EntropyCalculator.h" 
#include "../../backend/Module5_FrequencyAndHybrid/HybridImageBuilder.h"
#include "../../backend/Module5_FrequencyAndHybrid/FrequencyFilters.h" 
#include "../../backend/Module3_HistogramsAndColor/HistogramTools.h"
#include "../../backend/Module3_HistogramsAndColor/ColorTransformations.h"
#include "../../backend/Module3_HistogramsAndColor/ImageEqualizer.h"
#include "../../backend/Module3_HistogramsAndColor/ImageNormalizer.h"

#include <QInputDialog> 

AppController::AppController(MainWindow* window, QObject *parent)
    : QObject(parent), mainWindow(window) {
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::taskChanged, this, &AppController::handleTaskChange);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::applyRequested, this, &AppController::handleApply);
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
    cv::Mat currentImg = inputs[0]->getImage();

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
            
            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
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
        cv::Mat plot = HistogramTools::plotHistogram(hist, cdf, cv::Scalar(255, 255, 255));
        
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
        ImageEqualizer equalizer;

        QStringList items;
        items << "Grayscale Equalization" << "RGB Equalization";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Histogram Equalization", "Select Mode:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            cv::Mat result;

            if (item == "Grayscale Equalization") {
                 
                cv::Mat grayImg;
                if (currentImg.channels() > 1)
                    cv::cvtColor(currentImg, grayImg, cv::COLOR_BGR2GRAY);
                else
                    grayImg = currentImg;

                result = equalizer.equalize_grayScale(grayImg);

            } else if (item == "RGB Equalization") {
                
                if (currentImg.channels() == 3) {
                    result = equalizer.equalize_rgb(currentImg);
                } else {
                     
                    result = equalizer.equalize_grayScale(currentImg);
                }
            }

            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
        }
    }

    // --- TASK 7: ENTROPY ---
    else if (taskIndex == 7) {
        double entropy = EntropyCalculator::calculate(currentImg);
        cv::Mat histGraph = EntropyCalculator::plotHistogram(currentImg);
        if (!outputs.isEmpty()) outputs[0]->displayImage(histGraph);

        QString explanation = QString("<h3>Entropy Analysis</h3><hr><p><b>Value:</b> %1</p>").arg(entropy, 0, 'f', 4);
        mainWindow->getInfoSidebar()->setHtml(explanation);
    }

    // --- TASK 8: COLOR TO GRAY & RGB HISTOGRAMS ---
    else if (taskIndex == 8) {
        cv::Mat grayResult = ColorTransformations::convertToGray(currentImg);
        if (!outputs.isEmpty()) outputs[0]->displayImage(grayResult);
        ColorTransformations::analyzeRGB(currentImg);
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
            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
        }
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
