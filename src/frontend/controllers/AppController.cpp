#include "AppController.h"
#include "../MainWindow.h"
#include "../components/TopTaskBar.h"
#include "../components/ImagePanel.h"
#include "../../backend/Module2_EdgesAndEntropy/EdgeDetectors.h"
#include "../../backend/Module2_EdgesAndEntropy/EntropyCalculator.h" 
#include "../../backend/Module5_FrequencyAndHybrid/HybridImageBuilder.h"
#include "../../backend/Module5_FrequencyAndHybrid/FrequencyFilters.h" 
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

    // --- TASK 3: EDGE DETECTION ---
    if (taskIndex == 3) {
        if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;

        QStringList items;
        items << "Sobel (Manual)" << "Prewitt (Manual)" << "Roberts (Manual)" << "Canny (OpenCV)";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Edge Detection", "Select Edge Mask:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            cv::Mat result;
            if (item == "Sobel (Manual)") {
                result = EdgeDetectors::applySobel(inputs[0]->getImage());
            } 
            else if (item == "Prewitt (Manual)") {
                result = EdgeDetectors::applyPrewitt(inputs[0]->getImage());
            } 
            else if (item == "Roberts (Manual)") {
                result = EdgeDetectors::applyRoberts(inputs[0]->getImage());
            } 
            else if (item == "Canny (OpenCV)") {
                result = EdgeDetectors::applyCanny(inputs[0]->getImage(), 50.0, 150.0);
            }
            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
        }
    }

    // --- TASK 7: ENTROPY ---
    if (taskIndex == 7) {
        if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;

        double entropy = EntropyCalculator::calculate(inputs[0]->getImage());
        cv::Mat histGraph = EntropyCalculator::plotHistogram(inputs[0]->getImage());
        
        if (!outputs.isEmpty()) outputs[0]->displayImage(histGraph);

        // Build the HTML explanation for the sidebar
        QString explanation = QString("<h3>Entropy Analysis</h3>"
                                      "<hr>"
                                      "<p style='font-size: 16px;'><b>Calculated Value:</b> <span style='color: #d9534f;'>%1</span></p>")
                                      .arg(entropy, 0, 'f', 4);

        explanation += "<br><h4>What does this mean?</h4>";
        
        if (entropy < 4.0) {
            explanation += "<p><b>Low Entropy:</b> This image has very little contrast or detail. It likely consists of large, uniform areas of the same color.</p>";
        } else if (entropy >= 4.0 && entropy <= 7.0) {
            explanation += "<p><b>Moderate Entropy:</b> This image has a standard amount of detail, contrast, and structural variance.</p>";
        } else {
            explanation += "<p><b>High Entropy:</b> This image is highly complex, detailed, or noisy. The pixel values are scattered widely across the entire spectrum.</p>";
        }

        mainWindow->getInfoSidebar()->setHtml(explanation);
    }

    // --- TASK 9: FREQUENCY FILTERS ---
    if (taskIndex == 9) {
        if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;

        QStringList items;
        items << "Low Pass" << "High Pass";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Filter Choice", "Select Filter Type:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            FrequencyFilters::FilterType type = (item == "Low Pass") ? FrequencyFilters::LOW_PASS : FrequencyFilters::HIGH_PASS;
            cv::Mat result = FrequencyFilters::applyFilter(inputs[0]->getImage(), 50.0f, type);
            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
        }
    }

    // --- TASK 10: HYBRID IMAGES ---
    if (taskIndex == 10) {
        if (inputs.size() >= 2) {
            cv::Mat img1 = inputs[0]->getImage();
            cv::Mat img2 = inputs[1]->getImage();
            if (!img1.empty() && !img2.empty()) {
                cv::Mat result = HybridImageBuilder::createHybrid(img1, img2, 15);
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