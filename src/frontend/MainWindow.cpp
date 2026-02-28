#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "controllers/AppController.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupCustomLayout();
    
    appController = new AppController(this);
    rebuildPanels(1, 1, {"Input"}, {"Output"});
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupCustomLayout() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5); 
    mainLayout->setSpacing(5);

    taskBar = new TopTaskBar(this);
    mainLayout->addWidget(taskBar);
    
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(2); 

    leftSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter = new QSplitter(Qt::Vertical, this);
    
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);

    infoSidebar = new QTextBrowser(this);
    infoSidebar->setStyleSheet("background-color: #f8f9fa; padding: 15px; border-left: 1px solid #ccc; font-size: 14px;");
    infoSidebar->hide(); 
    mainSplitter->addWidget(infoSidebar);
    
    mainSplitter->setStretchFactor(0, 2); 
    mainSplitter->setStretchFactor(1, 2); 
    mainSplitter->setStretchFactor(2, 1); 

    mainLayout->addWidget(mainSplitter, 1);
    setCentralWidget(centralWidget);
} 

void MainWindow::updateLayoutForTask(int taskIndex) {
    if (infoSidebar) infoSidebar->hide();

    // 1. RESCUE THE CURRENT IMAGE BEFORE REBUILDING
    cv::Mat savedInput;
    if (!inputPanels.isEmpty() && !inputPanels[0]->getImage().empty()) {
        savedInput = inputPanels[0]->getImage().clone();
    }

    // 2. REBUILD UI BASED ON TASK
    if (taskIndex == 3) { rebuildPanels(1, 3, {"Input"}, {"X Gradient", "Y Gradient", "Final Magnitude"}); } 
    else if (taskIndex == 8) { rebuildPanels(1, 4, {"Input RGB"}, {"Grayscale", "Blue Hist", "Green Hist", "Red Hist"}); }
    else if (taskIndex == 10) { rebuildPanels(2, 1, {"Image (Low Freq)", "Image (High Freq)"}, {"Hybrid Result"}); } 
    else if (taskIndex == 9) { rebuildPanels(1, 1, {"Input Image"}, {"Filtered Result"}); }
    else if (taskIndex == 7) { 
        rebuildPanels(1, 1, {"Input Image"}, {"Pixel Distribution Graph"});
        infoSidebar->show(); 
        infoSidebar->setHtml("<h3>Entropy Analysis</h3><p>Upload an image and hit apply to see the results.</p>");
    }
    else { rebuildPanels(1, 1, {"Input"}, {"Output"}); }

    // 3. RESTORE THE RESCUED IMAGE
    if (!savedInput.empty() && !inputPanels.isEmpty()) {
        inputPanels[0]->displayImage(savedInput);
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