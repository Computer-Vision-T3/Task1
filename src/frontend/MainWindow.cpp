#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "controllers/AppController.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupCustomLayout();
    
    // Initialize controller AFTER setupCustomLayout
    appController = new AppController(this);
    
    // Initial state
    rebuildPanels(1, 1, {"Input"}, {"Output"});
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupCustomLayout() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5); // Tighten the edges
    mainLayout->setSpacing(5);

    taskBar = new TopTaskBar(this);
    mainLayout->addWidget(taskBar);
    
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1); // Thin line between left and right

    leftSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter = new QSplitter(Qt::Vertical, this);
    
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);
    
    // Set initial proportions (50% left, 50% right)
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(mainSplitter, 1);
    setCentralWidget(centralWidget);
} 

void MainWindow::updateLayoutForTask(int taskIndex) {
    // Index 10: Hybrid (2 Inputs, 1 Output)
    if (taskIndex == 10) {
        rebuildPanels(2, 1, {"Image (Low Freq)", "Image (High Freq)"}, {"Hybrid Result"});
    } 
    // Index 9: Frequency Filters (1 Input, 1 Output)
    else if (taskIndex == 9) {
        rebuildPanels(1, 1, {"Input Image"}, {"Filtered Result"});
    }
    // Default (1 Input, 1 Output)
    else {
        rebuildPanels(1, 1, {"Input"}, {"Output"});
    }
}

void MainWindow::rebuildPanels(int numInputs, int numOutputs, QStringList inTitles, QStringList outTitles) {
    qDeleteAll(inputPanels); inputPanels.clear();
    qDeleteAll(outputPanels); outputPanels.clear();

    for(int i = 0; i < numInputs; ++i) {
        QString title = (i < inTitles.size()) ? inTitles[i] : QString("Input %1").arg(i+1);
        ImagePanel* panel = new ImagePanel(title, true, this);
        leftSplitter->addWidget(panel);
        inputPanels.append(panel);
    }

    for(int i = 0; i < numOutputs; ++i) {
        QString title = (i < outTitles.size()) ? outTitles[i] : QString("Output %1").arg(i+1);
        ImagePanel* panel = new ImagePanel(title, false, this);
        rightSplitter->addWidget(panel);
        outputPanels.append(panel);
    }
}
