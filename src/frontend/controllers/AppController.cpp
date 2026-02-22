#include "AppController.h"
#include <QFileDialog>
#include <QMessageBox>

AppController::AppController(ImagePanel* inputP, ImagePanel* outputP, TopTaskBar* tb, QObject *parent)
    : QObject(parent), inputPanel(inputP), outputPanel(outputP), taskBar(tb) {
    
    connect(taskBar, &TopTaskBar::uploadRequested, this, &AppController::handleUpload);
    connect(taskBar, &TopTaskBar::applyRequested, this, &AppController::handleApply);
    connect(taskBar, &TopTaskBar::clearRequested, this, &AppController::handleClear);
}

void AppController::handleUpload() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Image", "", "Images (*.png *.xpm *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat image = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
        if(!image.empty()) {
            stateManager.setOriginalImage(image);
            inputPanel->setImage(image);
            outputPanel->clearImage();
        }
    }
}

void AppController::handleClear() {
    stateManager.triggerClear();
    inputPanel->setImage(stateManager.getOriginalImage()); // visually reset the input panel
}

void AppController::handleApply() {
    cv::Mat inputImage = stateManager.getNextInput();
    if(inputImage.empty()) {
        QMessageBox::warning(nullptr, "Warning", "Please upload an image first.");
        return;
    }

    int task = taskBar->getSelectedOperation();
    cv::Mat resultImage;

    // TODO: Connect to backend ImageProcessorInterface here
    // Example pseudo-code:
    // resultImage = Backend::Process(task, inputImage, parameters);
    
    // Fallback logic so the UI does something right now:
    cv::bitwise_not(inputImage, resultImage); // Just inverts the image as a placeholder test

    stateManager.setOutputImage(resultImage);
    inputPanel->setImage(inputImage); // Show what was fed in
    outputPanel->setImage(resultImage); // Show the result
}