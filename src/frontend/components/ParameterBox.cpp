// ParameterBox.cpp
#include "ParameterBox.h"

ParameterBox::ParameterBox(QWidget *parent) : QWidget(parent) {
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
}

void ParameterBox::clearLayout() {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void ParameterBox::updateParametersForTask(int taskIndex) {
    clearLayout();
    // Example: Dynamically generate UI based on task
    if (taskIndex == 0) { // Task 1: Noise
        layout->addWidget(new QLabel("Noise Type: Gaussian", this));
    } else {
        layout->addWidget(new QLabel("Parameters will load here", this));
    }
}