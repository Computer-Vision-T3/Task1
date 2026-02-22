#include "TopTaskBar.h"
#include <QHBoxLayout>

TopTaskBar::TopTaskBar(QWidget *parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    
    uploadBtn = new QPushButton("Upload Image", this);
    clearBtn = new QPushButton("Clear (Use Original)", this);
    applyBtn = new QPushButton("Apply Effect", this);
    
    operationSelector = new QComboBox(this);
    operationSelector->addItems({
        "1. Add Noise", "2. Low Pass Filter", "3. Edge Detection", "4. Histogram",
        "5. Equalization", "6. Normalization", "7. Entropy", "8. Color Transform",
        "9. Freq Filters", "10. Hybrid Images"
    });
    
    paramBox = new ParameterBox(this);
    
    layout->addWidget(uploadBtn);
    layout->addWidget(operationSelector);
    layout->addWidget(paramBox, 1);
    layout->addWidget(clearBtn);
    layout->addWidget(applyBtn);
    
    connect(uploadBtn, &QPushButton::clicked, this, &TopTaskBar::uploadRequested);
    connect(applyBtn, &QPushButton::clicked, this, &TopTaskBar::applyRequested);
    connect(clearBtn, &QPushButton::clicked, this, &TopTaskBar::clearRequested);
    
    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), paramBox, &ParameterBox::updateParametersForTask);
}

int TopTaskBar::getSelectedOperation() const {
    return operationSelector->currentIndex() + 1; // +1 to align with Task 1-10
}