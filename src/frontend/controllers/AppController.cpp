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
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::taskChanged,   this, &AppController::handleTaskChange);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::applyRequested, this, &AppController::handleApply);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::clearRequested, this, &AppController::handleClear);
    connect(mainWindow->getTopTaskBar(), &TopTaskBar::saveRequested,  this, &AppController::handleSave);
}

void AppController::handleTaskChange(int taskIndex) {
    mainWindow->updateLayoutForTask(taskIndex);
}

void AppController::handleApply() {
    int taskIndex = mainWindow->getTopTaskBar()->getSelectedOperation();
    auto& inputs  = mainWindow->getInputPanels();
    auto& outputs = mainWindow->getOutputPanels();

    if (inputs.isEmpty() || inputs[0]->getImage().empty()) {
        mainWindow->setStatusMessage("No image!", false);
        return;
    }

    mainWindow->getTopTaskBar()->setProcessing(true);
    cv::Mat currentImg = inputs[0]->getImage();
    ParameterBox* pBox = mainWindow->getTopTaskBar()->getParameterBox();

    // ── TASK 1: ADD NOISE ─────────────────────────────────
    if (taskIndex == 1) {
        QComboBox* combo  = pBox->findChild<QComboBox*>("noiseTypeCombo");
        QSlider*   slider = pBox->findChild<QSlider*>("noiseIntensitySlider");
        if (!combo || !slider) { mainWindow->getTopTaskBar()->setProcessing(false); return; }

        QString item = combo->currentText();
        int intensity = slider->value();
        cv::Mat result;

        if      (item == "Uniform")       result = NoiseGenerator::addUniformNoise(currentImg, intensity);
        else if (item == "Gaussian")      result = NoiseGenerator::addGaussianNoise(currentImg, intensity / 2.0);
        else if (item == "Salt & Pepper") result = NoiseGenerator::addSaltPepperNoise(currentImg, intensity / 100.0);

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // ── TASK 2: LOW PASS FILTERS ──────────────────────────
    else if (taskIndex == 2) {
        QComboBox* combo = pBox->findChild<QComboBox*>("filterTypeCombo");
        QSpinBox*  spin  = pBox->findChild<QSpinBox*>("kernelSizeSpin");
        if (!combo || !spin) { mainWindow->getTopTaskBar()->setProcessing(false); return; }

        QString item       = combo->currentText();
        int     kernelSize = spin->value();
        cv::Mat result;

        if      (item == "Average")  result = LowPassFilters::applyAverage(currentImg, kernelSize);
        else if (item == "Gaussian") result = LowPassFilters::applyGaussian(currentImg, kernelSize);
        else if (item == "Median")   result = LowPassFilters::applyMedian(currentImg, kernelSize);

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // ── TASK 3: EDGE DETECTION ────────────────────────────
    else if (taskIndex == 3) {
        QComboBox* combo = pBox->findChild<QComboBox*>("maskTypeCombo");
        if (!combo) { mainWindow->getTopTaskBar()->setProcessing(false); return; }
        QString item = combo->currentText();

        if (item == "Canny") {
            cv::Mat result = EdgeDetectors::applyCanny(currentImg, 50.0, 150.0);
            if (outputs.size() >= 3) {
                outputs[0]->clear();
                outputs[1]->clear();
                outputs[2]->displayImage(result);
            }
        } else {
            std::vector<cv::Mat> results;
            if      (item == "Sobel")   results = EdgeDetectors::applySobel(currentImg);
            else if (item == "Prewitt") results = EdgeDetectors::applyPrewitt(currentImg);
            else if (item == "Roberts") results = EdgeDetectors::applyRoberts(currentImg);

            if (outputs.size() >= 3 && results.size() == 3) {
                outputs[0]->displayImage(results[0]);
                outputs[1]->displayImage(results[1]);
                outputs[2]->displayImage(results[2]);
            }
        }
    }

    // ── TASK 4: HISTOGRAM ─────────────────────────────────
    else if (taskIndex == 4) {
        cv::Mat grayImg;
        if (currentImg.channels() > 1)
            grayImg = ColorTransformations::convertToGray(currentImg);
        else
            grayImg = currentImg;

        cv::Mat hist, cdf;
        HistogramTools::getHistogramAndCDF(grayImg, hist, cdf);
        cv::Mat plot = HistogramTools::plotHistogram(hist, cdf, cv::Scalar(155, 140, 230)); // violet

        if (!outputs.isEmpty()) outputs[0]->displayImage(plot);
    }

    // ── TASK 5: NORMALIZE ─────────────────────────────────
    else if (taskIndex == 5) {
        ImageNormalizer normalizer;
        cv::Mat normalizedFloat   = normalizer.normalize_image(currentImg);
        cv::Mat normalizedDisplay;
        normalizedFloat.convertTo(normalizedDisplay, CV_8U, 255.0);
        if (!outputs.isEmpty()) outputs[0]->displayImage(normalizedDisplay);
    }

    // ── TASK 6: EQUALIZE ──────────────────────────────────
    else if (taskIndex == 6) {
        QComboBox* combo = pBox->findChild<QComboBox*>("eqModeCombo");
        if (!combo) { mainWindow->getTopTaskBar()->setProcessing(false); return; }

        ImageEqualizer equalizer;
        cv::Mat result;

        if (combo->currentText() == "Grayscale Equalization") {
            cv::Mat grayImg;
            if (currentImg.channels() > 1) cv::cvtColor(currentImg, grayImg, cv::COLOR_BGR2GRAY);
            else grayImg = currentImg;
            result = equalizer.equalize_grayScale(grayImg);
        } else {
            result = (currentImg.channels() == 3)
                ? equalizer.equalize_rgb(currentImg)
                : equalizer.equalize_grayScale(currentImg);
        }

        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // ── TASK 7: ENTROPY ───────────────────────────────────
    else if (taskIndex == 7) {
        double entropy    = EntropyCalculator::calculate(currentImg);
        cv::Mat histGraph = EntropyCalculator::plotHistogram(currentImg);
        if (!outputs.isEmpty()) outputs[0]->displayImage(histGraph);

        // Classify entropy level
        QString levelColor, levelName, levelDesc;
        if (entropy < 4.5) {
            levelColor = "#D46B3C";
            levelName  = "Low";
            levelDesc  = "Uniform areas dominate — flat sky, solid backgrounds, low structural detail.";
        } else if (entropy < 6.8) {
            levelColor = "#5B4FCF";
            levelName  = "Moderate";
            levelDesc  = "Typical photograph with natural variance in lighting and subject matter.";
        } else {
            levelColor = "#2D9B6F";
            levelName  = "High";
            levelDesc  = "Rich texture, complex detail, or high-frequency noise present.";
        }

        QString html = QString(R"(
            <style>
                body { font-family: 'DM Sans', 'Segoe UI', sans-serif; color: #2C2825; margin: 0; padding: 0; }
                .card { background: #FAFAF8; border-radius: 10px; padding: 14px 16px; margin-bottom: 12px; }
                .card h4 { margin: 0 0 6px; font-size: 11px; font-weight: 700; letter-spacing: 0.1em; color: #A09890; text-transform: uppercase; }
                .val { font-size: 26px; font-weight: 900; color: %1; letter-spacing: -0.02em; }
                .badge { display: inline-block; background: %1; color: #fff; border-radius: 6px; padding: 3px 10px; font-size: 11px; font-weight: 800; margin-bottom: 8px; }
                .desc { font-size: 12px; color: #7A7268; line-height: 1.6; }
                .formula { font-size: 13px; font-weight: 700; color: #5B4FCF; background: #EDE8FF; border-radius: 6px; padding: 6px 12px; display: inline-block; margin-top: 6px; }
                .range-row { display: flex; margin: 4px 0; }
                .range-dot { width: 8px; height: 8px; border-radius: 4px; margin: 4px 8px 0 0; flex-shrink: 0; }
                .range-txt { font-size: 11px; color: #7A7268; }
            </style>
            <div class='card'>
                <h4>Shannon Entropy</h4>
                <div class='val'>%2</div>
                <br>
                <span class='badge'>%3</span>
                <p class='desc'>%4</p>
            </div>
            <div class='card'>
                <h4>Reference Scale</h4>
                <div class='range-row'><div class='range-dot' style='background:#D46B3C'></div><div class='range-txt'>&lt; 4.5 · Low — uniform images</div></div>
                <div class='range-row'><div class='range-dot' style='background:#5B4FCF'></div><div class='range-txt'>4.5–6.8 · Moderate — natural photos</div></div>
                <div class='range-row'><div class='range-dot' style='background:#2D9B6F'></div><div class='range-txt'>&gt; 6.8 · High — complex/noisy</div></div>
            </div>
            <div class='card'>
                <h4>Formula</h4>
                <div class='formula'>H = −Σ pᵢ log₂(pᵢ)</div>
                <p class='desc' style='margin-top: 8px;'>Computed from a manually built 256-bin histogram without OpenCV's calcHist.</p>
            </div>
        )").arg(levelColor).arg(entropy, 0, 'f', 4).arg(levelName).arg(levelDesc);

        mainWindow->getInfoSidebar()->setHtml(html);
    }

    // ── TASK 8: COLOR TRANSFORM ───────────────────────────
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

    // ── TASK 9: FREQUENCY FILTERS ─────────────────────────
    else if (taskIndex == 9) {
        QComboBox* combo = pBox->findChild<QComboBox*>("freqTypeCombo");
        if (!combo) { mainWindow->getTopTaskBar()->setProcessing(false); return; }

        FrequencyFilters::FilterType type =
            (combo->currentText() == "Low Pass") ? FrequencyFilters::LOW_PASS : FrequencyFilters::HIGH_PASS;
        cv::Mat result = FrequencyFilters::applyFilter(currentImg, 50.0f, type);
        if (!outputs.isEmpty()) outputs[0]->displayImage(result);
    }

    // ── TASK 10: HYBRID IMAGES ────────────────────────────
    else if (taskIndex == 10) {
        if (inputs.size() >= 2) {
            cv::Mat img2 = inputs[1]->getImage();
            if (!currentImg.empty() && !img2.empty()) {
                cv::Mat result = HybridImageBuilder::createHybrid(currentImg, img2, 15);
                if (!outputs.isEmpty()) outputs[0]->displayImage(result);
            } else {
                mainWindow->setStatusMessage("Need 2 images!", false);
                mainWindow->getTopTaskBar()->setProcessing(false);
                return;
            }
        }
    }

    mainWindow->getTopTaskBar()->setProcessing(false);
    mainWindow->setStatusMessage("Done ✓", true);
}

void AppController::handleClear() {
    for (auto* panel : mainWindow->getOutputPanels()) panel->clear();
    mainWindow->setStatusMessage("Cleared", true);
}

void AppController::handleSave() {
    auto& outputs = mainWindow->getOutputPanels();
    if (outputs.isEmpty() || outputs[0]->getImage().empty()) {
        mainWindow->setStatusMessage("Nothing to save", false);
        return;
    }

    QString filter   = "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)";
    QString fileName = QFileDialog::getSaveFileName(
        mainWindow, "Save Processed Image", "", filter
    );

    if (!fileName.isEmpty()) {
        bool ok = cv::imwrite(fileName.toStdString(), outputs[0]->getImage());
        if (ok) {
            mainWindow->setStatusMessage("Saved ✓", true);
        } else {
            mainWindow->setStatusMessage("Save failed", false);
            QMessageBox::critical(mainWindow, "Save Error",
                "Could not write the file. Check path and extension.");
        }
    }
}
