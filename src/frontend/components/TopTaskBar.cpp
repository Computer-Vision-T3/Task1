#include "TopTaskBar.h"
#include <QHBoxLayout>

TopTaskBar::TopTaskBar(QWidget *parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8); 
    
    clearBtn = new QPushButton("Clear (Use Original)", this);
    saveBtn = new QPushButton("Save Output", this);
    applyBtn = new QPushButton("Apply Effect", this);
    
    // --- THE HELP BUBBLE ---
    helpBubble = new QLabel("?", this);
    helpBubble->setObjectName("helpBubble");
    helpBubble->setAlignment(Qt::AlignCenter);
    helpBubble->setCursor(Qt::WhatsThisCursor);
    
    operationSelector = new QComboBox(this);
    operationSelector->addItems({
        "1. Add Noise", "2. Low Pass Filter", "3. Edge Detection", "4. Histogram",
        "5. Normalization", "6. Equalization", "7. Entropy", "8. Color Transform",
        "9. Freq Filters", "10. Hybrid Images"
    });
    
    paramBox = new ParameterBox(this);
    
    layout->addWidget(operationSelector);
    layout->addWidget(helpBubble); // Insert bubble next to the dropdown
    layout->addWidget(paramBox, 1); 
    layout->addWidget(clearBtn);
    layout->addWidget(saveBtn);
    layout->addWidget(applyBtn);
    
    connect(applyBtn, &QPushButton::clicked, this, &TopTaskBar::applyRequested);
    connect(saveBtn, &QPushButton::clicked, this, &TopTaskBar::saveRequested);
    connect(clearBtn, &QPushButton::clicked, this, &TopTaskBar::clearRequested);
    
    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            paramBox, &ParameterBox::updateParametersForTask);
            
    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this](int index){
        updateHelpText(index + 1);
        emit taskChanged(index + 1); 
    });

    paramBox->updateParametersForTask(operationSelector->currentIndex());
    updateHelpText(1); // Initialize the first help text
}

int TopTaskBar::getSelectedOperation() const {
    return operationSelector->currentIndex() + 1;
}

void TopTaskBar::updateHelpText(int taskIndex) {
    QString helpTxt;
    switch(taskIndex) {
        case 1: helpTxt = "<b>Add Noise:</b> Simulates camera sensors or transmission errors. <br><i>Uniform</i> applies random static. <br><i>Gaussian</i> applies bell-curve noise. <br><i>Salt & Pepper</i> adds black and white dots."; break;
        case 2: helpTxt = "<b>Low Pass Filter:</b> Smooths images and removes noise. <br><i>Average</i> blurs everything. <br><i>Gaussian</i> preserves edges slightly better. <br><i>Median</i> is perfect for removing Salt & Pepper noise."; break;
        case 3: helpTxt = "<b>Edge Detection:</b> Finds sharp color transitions. <br>Outputs show Vertical lines (X), Horizontal lines (Y), and combined contours (Magnitude)."; break;
        case 4: helpTxt = "<b>Histogram:</b> Plots pixel brightness. X-axis is color (0-255), Y-axis is pixel count. The white line is the Cumulative Distribution Function (CDF)."; break;
        case 5: helpTxt = "<b>Normalization:</b> Mathematically stretches the darkest pixel to pure black (0) and brightest to pure white (255) to fix lighting."; break;
        case 6: helpTxt = "<b>Equalization:</b> Flattens the histogram to dramatically boost contrast in washed-out photos."; break;
        case 7: helpTxt = "<b>Entropy:</b> Measures randomness. High entropy = lots of textures/details. Low entropy = flat, solid colors."; break;
        case 8: helpTxt = "<b>Color Transform:</b> Splits an image into its Red, Green, and Blue light channels and graphs their separate histograms."; break;
        case 9: helpTxt = "<b>Frequency Filters:</b> Uses Fourier Transforms. Low-pass blurs the image. High-pass extracts the sharp edges."; break;
        case 10: helpTxt = "<b>Hybrid Images:</b> Combines the blurry shape of Image 1 with the sharp edges of Image 2. Move away from the screen to see the blurry one!"; break;
    }
    helpBubble->setToolTip(helpTxt);
}