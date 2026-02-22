#include "AppController.h"
#include <QMessageBox>

AppController::AppController(MainWindow* window, QObject *parent)
    : QObject(parent), mainWindow(window) {
    
    connect(mainWindow->taskBar, &TopTaskBar::applyRequested, this, &AppController::handleApply);
    connect(mainWindow->taskBar, &TopTaskBar::clearRequested, this, &AppController::handleClear);
    connect(mainWindow->taskBar, &TopTaskBar::taskChanged, this, &AppController::handleTaskChange);
}

void AppController::handleTaskChange(int taskIndex) {
    // DYNAMIC UI ROUTING
    if (taskIndex == 10) { 
        // Task 10: Hybrid (Needs 2 inputs, 1 output)
        mainWindow->rebuildPanels(2, 1, {"Image 1 (High Freq)", "Image 2 (Low Freq)"}, {"Hybrid Output"});
    } 
    else if (taskIndex == 8) { 
        // Task 8: Color Transform (1 input, 4 outputs)
        mainWindow->rebuildPanels(1, 4, {"RGB Input"}, {"Grayscale", "R Histogram", "G Histogram", "B Histogram"});
    } 
    else { 
        // Default: 1 input, 1 output
        mainWindow->rebuildPanels(1, 1, {"Input Image"}, {"Output Result"});
    }
}

void AppController::handleClear() {
    stateManager.triggerClear();
    // Clear all outputs visually
    for(auto* out : mainWindow->outputPanels) { out->clearImage(); }
}

void AppController::handleApply() {
    int task = mainWindow->taskBar->getSelectedOperation();
    
    // Example validation: Check if input 1 is loaded
    if(mainWindow->inputPanels.isEmpty() || mainWindow->inputPanels[0]->getCurrentImage().empty()) {
        QMessageBox::warning(nullptr, "Warning", "Please load the required input images first.");
        return;
    }

    // TODO: Pass mainWindow->inputPanels to backend interface
    // Fallback placeholder logic just to prove it works:
    cv::Mat resultImage;
    cv::bitwise_not(mainWindow->inputPanels[0]->getCurrentImage(), resultImage);
    
    if(!mainWindow->outputPanels.isEmpty()) {
        mainWindow->outputPanels[0]->setImage(resultImage);
    }
}