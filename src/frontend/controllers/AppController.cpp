#include "AppController.h"
#include "../MainWindow.h"
#include "../components/TopTaskBar.h"
#include "../components/ImagePanel.h"
#include "../../backend/Module5_FrequencyAndHybrid/HybridImageBuilder.h"
#include "../../backend/Module5_FrequencyAndHybrid/FrequencyFilters.h" // Add this
#include <QInputDialog> // For a quick selection list

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

    // --- FREQUENCY FILTER LOGIC (Index 9) ---
    if (taskIndex == 9) {
        if (inputs.isEmpty() || inputs[0]->getImage().empty()) return;

        // 1. Let user choose type via a list
        QStringList items;
        items << "Low Pass" << "High Pass";
        bool ok;
        QString item = QInputDialog::getItem(mainWindow, "Filter Choice", "Select Filter Type:", items, 0, false, &ok);

        if (ok && !item.isEmpty()) {
            FrequencyFilters::FilterType type = (item == "Low Pass") ? FrequencyFilters::LOW_PASS : FrequencyFilters::HIGH_PASS;
            
            // 2. Apply Filter (D0 = 50)
            cv::Mat result = FrequencyFilters::applyFilter(inputs[0]->getImage(), 50.0f, type);
            
            if (!outputs.isEmpty()) outputs[0]->displayImage(result);
        }
    }

    // --- HYBRID IMAGE LOGIC (Index 10) ---
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
