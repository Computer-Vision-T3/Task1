#include "TopTaskBar.h"
#include <QHBoxLayout>

TopTaskBar::TopTaskBar(QWidget *parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5); // Give it a little breathing room
    
    // 1. Create the controls
    clearBtn = new QPushButton("Clear (Use Original)", this);
    saveBtn = new QPushButton("Save Output", this);
    applyBtn = new QPushButton("Apply Effect", this);
    
    operationSelector = new QComboBox(this);
    operationSelector->addItems({
        "1. Add Noise", "2. Low Pass Filter", "3. Edge Detection", "4. Histogram",
        "5. Equalization", "6. Normalization", "7. Entropy", "8. Color Transform",
        "9. Freq Filters", "10. Hybrid Images"
    });
    
    paramBox = new ParameterBox(this);
    
    // 2. Add them to the layout
    layout->addWidget(operationSelector);
    layout->addWidget(paramBox, 1); // The '1' makes the parameter box stretch to fill empty space
    layout->addWidget(clearBtn);
    layout->addWidget(saveBtn);
    layout->addWidget(applyBtn);
    
    // 3. Connect the buttons to the outbound signals
    connect(applyBtn, &QPushButton::clicked, this, &TopTaskBar::applyRequested);
    connect(saveBtn, &QPushButton::clicked, this, &TopTaskBar::saveRequested);
    connect(clearBtn, &QPushButton::clicked, this, &TopTaskBar::clearRequested);
    
    // 4. Connect the dropdown to update the UI layouts AND the ParameterBox
    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            paramBox, &ParameterBox::updateParametersForTask);
            
    connect(operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this](int index){
        emit taskChanged(index + 1); // +1 aligns the 0-based index with your Task 1-10 numbering
    });

    paramBox->updateParametersForTask(operationSelector->currentIndex());
}

int TopTaskBar::getSelectedOperation() const {
    return operationSelector->currentIndex() + 1;
}