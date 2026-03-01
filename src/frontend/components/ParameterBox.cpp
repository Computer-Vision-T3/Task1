#include "ParameterBox.h"
#include <QFrame>
#include <QTimer>

ParameterBox::ParameterBox(QWidget *parent) : QWidget(parent) {
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 6, 12, 6);
    layout->setSpacing(6);
}

void ParameterBox::clearLayout() {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}

QFrame* ParameterBox::buildSeparator() {
    QFrame* sep = new QFrame(this);
    sep->setObjectName("vertDivider");
    sep->setFrameShape(QFrame::VLine);
    sep->setFixedWidth(1);
    sep->setFixedHeight(28);
    return sep;
}

QWidget* ParameterBox::buildLabeledCombo(const QString& label, const QString& objName,
                                          const QStringList& items, const QString& tooltip) {
    QWidget* container = new QWidget(this);
    QHBoxLayout* cl = new QHBoxLayout(container);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(8);

    QLabel* lbl = new QLabel(label, container);
    lbl->setObjectName("paramLabel");
    lbl->setFixedWidth(label.length() * 7 + 4);

    QComboBox* combo = new QComboBox(container);
    combo->setObjectName(objName);
    combo->addItems(items);
    combo->setMinimumWidth(140);
    if (!tooltip.isEmpty()) combo->setToolTip(tooltip);

    cl->addWidget(lbl);
    cl->addWidget(combo);
    return container;
}

QWidget* ParameterBox::buildLabeledSlider(const QString& label, const QString& objName,
                                           int min, int max, int value) {
    QWidget* container = new QWidget(this);
    QHBoxLayout* cl = new QHBoxLayout(container);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(8);

    QLabel* lbl = new QLabel(label, container);
    lbl->setObjectName("paramLabel");
    lbl->setFixedWidth(label.length() * 7 + 4);

    QSlider* slider = new QSlider(Qt::Horizontal, container);
    slider->setObjectName(objName);
    slider->setRange(min, max);
    slider->setValue(value);
    slider->setFixedWidth(130);

    QLabel* valLabel = new QLabel(QString::number(value), container);
    valLabel->setObjectName("valueLabel");
    valLabel->setFixedWidth(32);
    valLabel->setAlignment(Qt::AlignCenter);

    // Live value display
    QObject::connect(slider, &QSlider::valueChanged, [valLabel](int v) {
        valLabel->setText(QString::number(v));
    });

    cl->addWidget(lbl);
    cl->addWidget(slider);
    cl->addWidget(valLabel);
    return container;
}

QWidget* ParameterBox::buildLabeledSpin(const QString& label, const QString& objName,
                                         int min, int max, int step, int value) {
    QWidget* container = new QWidget(this);
    QHBoxLayout* cl = new QHBoxLayout(container);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(8);

    QLabel* lbl = new QLabel(label, container);
    lbl->setObjectName("paramLabel");
    lbl->setFixedWidth(label.length() * 7 + 4);

    QSpinBox* spin = new QSpinBox(container);
    spin->setObjectName(objName);
    spin->setRange(min, max);
    spin->setSingleStep(step);
    spin->setValue(value);
    spin->setFixedWidth(80);

    cl->addWidget(lbl);
    cl->addWidget(spin);
    return container;
}

void ParameterBox::updateParametersForTask(int taskIndex) {
    clearLayout();

    switch (taskIndex) {
        case 0: { // Task 1: Add Noise
            layout->addWidget(buildLabeledCombo(
                "Type", "noiseTypeCombo",
                {"Uniform", "Gaussian", "Salt & Pepper"},
                "Uniform: flat random  |  Gaussian: bell-curve (ISO simulation)  |  Salt & Pepper: dead pixels"
            ));
            layout->addWidget(buildSeparator());
            layout->addWidget(buildLabeledSlider("Intensity", "noiseIntensitySlider", 0, 100, 20));

            // Dynamic tooltip hint
            QLabel* hint = new QLabel("", this);
            hint->setStyleSheet("font-size: 10px; color: #B8B0A6; font-style: italic;");
            hint->setObjectName("noiseHint");

            auto* combo = this->findChild<QComboBox*>("noiseTypeCombo");
            if (combo) {
                auto updateHint = [hint](const QString& t) {
                    if (t == "Uniform") hint->setText("Adds uniform random noise");
                    else if (t == "Gaussian") hint->setText("σ = intensity / 2");
                    else hint->setText("fraction = intensity / 100");
                };
                QObject::connect(combo, &QComboBox::currentTextChanged, updateHint);
                updateHint(combo->currentText());
            }
            layout->addWidget(hint);
            layout->addStretch();
            break;
        }
        case 1: { // Task 2: Low Pass Filters
            layout->addWidget(buildLabeledCombo(
                "Filter", "filterTypeCombo",
                {"Average", "Gaussian", "Median"},
                "Average: mean of neighborhood  |  Gaussian: weighted center-biased  |  Median: best for salt & pepper"
            ));
            layout->addWidget(buildSeparator());
            layout->addWidget(buildLabeledSpin("Kernel", "kernelSizeSpin", 3, 31, 2, 3));

            QLabel* hint = new QLabel("Larger kernel → stronger blur", this);
            hint->setStyleSheet("font-size: 10px; color: #B8B0A6; font-style: italic;");
            layout->addWidget(hint);
            layout->addStretch();
            break;
        }
        case 2: { // Task 3: Edge Detection
            layout->addWidget(buildLabeledCombo(
                "Detector", "maskTypeCombo",
                {"Sobel", "Roberts", "Prewitt", "Canny"},
                "Sobel: standard 3×3  |  Roberts: diagonal 2×2  |  Prewitt: uniform weights  |  Canny: multi-stage, 1px edges"
            ));

            QLabel* hint = new QLabel("Outputs: X Gradient · Y Gradient · Magnitude", this);
            hint->setStyleSheet("font-size: 10px; color: #B8B0A6; font-style: italic;");
            layout->addWidget(hint);
            layout->addStretch();
            break;
        }
        case 5: { // Task 6: Equalization
            layout->addWidget(buildLabeledCombo(
                "Mode", "eqModeCombo",
                {"Grayscale Equalization", "RGB Equalization"},
                "Grayscale: luminance CDF mapping  |  RGB: per-channel equalization (may shift hues)"
            ));
            layout->addStretch();
            break;
        }
        case 8: { // Task 9: Frequency Filters
            layout->addWidget(buildLabeledCombo(
                "Type", "freqTypeCombo",
                {"Low Pass", "High Pass"},
                "Low pass: smoothing/blur  |  High pass: edge extraction"
            ));

            QLabel* hint = new QLabel("Gaussian Fourier filter  ·  D₀ = 50", this);
            hint->setStyleSheet("font-size: 10px; color: #B8B0A6; font-style: italic;");
            layout->addWidget(hint);
            layout->addStretch();
            break;
        }
        default: {
            QLabel* hint = new QLabel("No parameters required", this);
            hint->setStyleSheet("font-size: 12px; color: #C4BDB4; font-style: italic;");
            layout->addWidget(hint);
            layout->addStretch();
            break;
        }
    }
}
