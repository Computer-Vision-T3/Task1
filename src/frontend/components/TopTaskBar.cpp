#include "TopTaskBar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>

TopTaskBar::TopTaskBar(QWidget *parent) : QWidget(parent) {
    setObjectName("paramStrip");
    setFixedHeight(90);   // Two rows: 44px + 1px divider + 44px

    statusTimer = new QTimer(this);
    statusTimer->setSingleShot(true);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ════════════════════════════
    //  ROW 1 — Operation + Params
    // ════════════════════════════
    QWidget* row1 = new QWidget(this);
    row1->setFixedHeight(44);
    row1->setStyleSheet("background-color: #FAFAF8;");
    QHBoxLayout* r1 = new QHBoxLayout(row1);
    r1->setContentsMargins(16, 0, 16, 0);
    r1->setSpacing(8);

    QLabel* opLabel = new QLabel("Operation", row1);
    opLabel->setObjectName("paramLabel");
    opLabel->setFixedWidth(72);

    operationSelector = new QComboBox(row1);
    operationSelector->setObjectName("operationSelector");
    operationSelector->setFixedWidth(178);
    operationSelector->setFixedHeight(32);
    operationSelector->addItems({
        "1  \xc2\xb7  Add Noise",
        "2  \xc2\xb7  Low Pass Filter",
        "3  \xc2\xb7  Edge Detection",
        "4  \xc2\xb7  Histogram",
        "5  \xc2\xb7  Normalization",
        "6  \xc2\xb7  Equalization",
        "7  \xc2\xb7  Entropy Analysis",
        "8  \xc2\xb7  Color Transform",
        "9  \xc2\xb7  Freq. Filters",
        "10  \xc2\xb7  Hybrid Images"
    });

    helpBubble = new QLabel("?", row1);
    helpBubble->setObjectName("helpBubble");
    helpBubble->setAlignment(Qt::AlignCenter);
    helpBubble->setCursor(Qt::WhatsThisCursor);
    helpBubble->setFixedSize(24, 24);
    helpBubble->setStyleSheet(
        "background-color:#EDE8FF; color:#5B4FCF;"
        "border-radius:12px; font-weight:800; font-size:13px; padding:0px;"
    );

    QFrame* div1 = new QFrame(row1);
    div1->setFrameShape(QFrame::VLine);
    div1->setFixedSize(1, 26);
    div1->setStyleSheet("background-color:#DDD8D2; border:none;");

    paramBox = new ParameterBox(row1);
    paramBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    r1->addWidget(opLabel);
    r1->addWidget(operationSelector);
    r1->addWidget(helpBubble);
    r1->addWidget(div1);
    r1->addWidget(paramBox, 1);

    // ════════════════════════════
    //  Separator
    // ════════════════════════════
    QFrame* rowSep = new QFrame(this);
    rowSep->setFrameShape(QFrame::HLine);
    rowSep->setFixedHeight(1);
    rowSep->setStyleSheet("background-color:#EDEAE4; border:none;");

    // ════════════════════════════
    //  ROW 2 — Desc + Status + Buttons
    // ════════════════════════════
    QWidget* row2 = new QWidget(this);
    row2->setFixedHeight(44);
    row2->setStyleSheet("background-color:#F7F5F2;");
    QHBoxLayout* r2 = new QHBoxLayout(row2);
    r2->setContentsMargins(16, 0, 16, 0);
    r2->setSpacing(8);

    taskDescLabel = new QLabel("", row2);
    taskDescLabel->setStyleSheet(
        "font-size:11px; color:#B8B0A6; font-style:italic; background:transparent;"
    );
    taskDescLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    statusLabel = new QLabel("Ready", row2);
    statusLabel->setFixedHeight(26);
    statusLabel->setMinimumWidth(60);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "background-color:#F0ECE6; color:#A09890;"
        "border-radius:6px; padding:0 10px;"
        "font-size:11px; font-weight:600;"
    );

    QFrame* div2 = new QFrame(row2);
    div2->setFrameShape(QFrame::VLine);
    div2->setFixedSize(1, 26);
    div2->setStyleSheet("background-color:#DDD8D2; border:none;");

    clearBtn = new QPushButton("Clear", row2);
    clearBtn->setObjectName("ghostBtn");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setFixedSize(72, 30);
    clearBtn->setToolTip("Clear outputs and return to original");

    saveBtn = new QPushButton("Save", row2);
    saveBtn->setObjectName("secondaryBtn");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setFixedSize(80, 30);
    saveBtn->setToolTip("Save the current output image");

    applyBtn = new QPushButton("Apply", row2);
    applyBtn->setObjectName("primaryBtn");
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setFixedSize(100, 30);
    applyBtn->setToolTip("Apply the selected operation (Enter)");

    r2->addWidget(taskDescLabel, 1);
    r2->addWidget(statusLabel);
    r2->addWidget(div2);
    r2->addWidget(clearBtn);
    r2->addWidget(saveBtn);
    r2->addWidget(applyBtn);

    // ════════════════════════════
    //  Assemble
    // ════════════════════════════
    outerLayout->addWidget(row1);
    outerLayout->addWidget(rowSep);
    outerLayout->addWidget(row2);

    // ── Connections ──────────────────────────────────────
    connect(applyBtn, &QPushButton::clicked, this, &TopTaskBar::applyRequested);
    connect(saveBtn,  &QPushButton::clicked, this, &TopTaskBar::saveRequested);
    connect(clearBtn, &QPushButton::clicked, this, &TopTaskBar::clearRequested);

    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            paramBox, &ParameterBox::updateParametersForTask);

    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        updateHelpText(index + 1);
        emit taskChanged(index + 1);
    });

    connect(statusTimer, &QTimer::timeout, [this]() {
        statusLabel->setText("Ready");
        statusLabel->setStyleSheet(
            "background-color:#F0ECE6; color:#A09890;"
            "border-radius:6px; padding:0 10px;"
            "font-size:11px; font-weight:600;"
        );
    });

    paramBox->updateParametersForTask(operationSelector->currentIndex());
    updateHelpText(1);
}

int TopTaskBar::getSelectedOperation() const {
    return operationSelector->currentIndex() + 1;
}

void TopTaskBar::setStatus(const QString& msg, bool isSuccess) {
    statusLabel->setText(msg);
    statusLabel->setStyleSheet(isSuccess
        ? "background-color:#E0F5EC; color:#1E8A5E; border-radius:6px; padding:0 10px; font-size:11px; font-weight:700;"
        : "background-color:#FFE8E8; color:#C0392B; border-radius:6px; padding:0 10px; font-size:11px; font-weight:700;"
    );
    statusTimer->start(2500);
}

void TopTaskBar::setProcessing(bool processing) {
    applyBtn->setEnabled(!processing);
    applyBtn->setText(processing ? "Working..." : "Apply");
    if (processing) {
        statusLabel->setText("Running");
        statusLabel->setStyleSheet(
            "background-color:#FFF5E0; color:#C47A0A;"
            "border-radius:6px; padding:0 10px; font-size:11px; font-weight:700;"
        );
    }
}

void TopTaskBar::updateHelpText(int taskIndex) {
    struct Info { const char* tip; const char* desc; };
    static const Info data[] = {
        { "<b>Add Noise:</b> Simulates sensor imperfections.<br>"
          "<i>Uniform:</i> flat static &nbsp;|&nbsp; <i>Gaussian:</i> ISO camera noise &nbsp;|&nbsp; <i>S&P:</i> dead pixels",
          "Introduces artificial noise to simulate real-world image degradation" },

        { "<b>Low Pass Filter:</b> Smoothing and noise removal.<br>"
          "<i>Average:</i> simple mean &nbsp;|&nbsp; <i>Gaussian:</i> edge-preserving &nbsp;|&nbsp; <i>Median:</i> destroys salt & pepper",
          "Smooths the image by averaging pixel neighborhoods — larger kernel = stronger blur" },

        { "<b>Edge Detection:</b> Locates intensity gradients.<br>"
          "Outputs: X gradient &nbsp;|&nbsp; Y gradient &nbsp;|&nbsp; combined Magnitude",
          "Detects sharp intensity boundaries — outputs X, Y gradients and combined magnitude" },

        { "<b>Histogram:</b> Pixel intensity distribution (0-255).<br>"
          "White line = Cumulative Distribution Function (CDF)",
          "Plots the distribution of pixel intensities with the CDF overlay" },

        { "<b>Normalization:</b> Stretches darkest to 0, brightest to 255.<br>"
          "Fixes exposure and contrast issues mathematically",
          "Linearly rescales pixel values: darkest maps to 0, brightest to 255" },

        { "<b>Equalization:</b> Flattens histogram via CDF mapping.<br>"
          "Dramatically improves low-contrast photos",
          "Redistributes intensities to produce a flat histogram and boost contrast" },

        { "<b>Entropy Analysis:</b> Measures information content (Shannon H).<br>"
          "Wide flat histogram = high entropy &nbsp;|&nbsp; Spiky = low entropy",
          "Calculates Shannon entropy H = -sum( p * log2(p) ) and plots the pixel distribution" },

        { "<b>Color Transform:</b> Splits image into R, G, B channels.<br>"
          "Shows each channel's individual histogram + CDF curve",
          "Separates the image into Red, Green and Blue channels with individual histograms" },

        { "<b>Frequency Filters:</b> Operates in the Fourier domain.<br>"
          "<i>Low pass:</i> blur/smooth &nbsp;|&nbsp; <i>High pass:</i> edge extraction",
          "Applies a Gaussian mask in frequency domain — low pass blurs, high pass extracts edges" },

        { "<b>Hybrid Images:</b> Blends two images at different frequencies.<br>"
          "Low-freq from Image 1 + High-freq from Image 2 — perception shifts with viewing distance",
          "Combines the coarse structure of one image with the fine detail of another" },
    };

    int idx = taskIndex - 1;
    if (idx >= 0 && idx < 10) {
        helpBubble->setToolTip(data[idx].tip);
        if (taskDescLabel) taskDescLabel->setText(data[idx].desc);
    }
}