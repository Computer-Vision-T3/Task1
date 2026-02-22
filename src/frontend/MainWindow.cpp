#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "controllers/AppController.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupCustomLayout();
    appController = new AppController(this);
    
    // Default boot state: 1 input, 1 output
    rebuildPanels(1, 1, {"Input"}, {"Output"});
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupCustomLayout() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    taskBar = new TopTaskBar(this);
    mainLayout->addWidget(taskBar);
    
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    leftSplitter = new QSplitter(Qt::Vertical, this);  // Inputs stack vertically
    rightSplitter = new QSplitter(Qt::Vertical, this); // Outputs stack vertically
    
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);
    mainLayout->addWidget(mainSplitter, 1);
    
    setCentralWidget(centralWidget);
}

void MainWindow::rebuildPanels(int numInputs, int numOutputs, QStringList inTitles, QStringList outTitles) {
    // Clear old panels out of memory
    qDeleteAll(inputPanels); inputPanels.clear();
    qDeleteAll(outputPanels); outputPanels.clear();

    // Spawn new Input panels
    for(int i = 0; i < numInputs; ++i) {
        QString title = (i < inTitles.size()) ? inTitles[i] : QString("Input %1").arg(i+1);
        ImagePanel* panel = new ImagePanel(title, true, this);
        leftSplitter->addWidget(panel);
        inputPanels.append(panel);
    }

    // Spawn new Output panels
    for(int i = 0; i < numOutputs; ++i) {
        QString title = (i < outTitles.size()) ? outTitles[i] : QString("Output %1").arg(i+1);
        ImagePanel* panel = new ImagePanel(title, false, this);
        rightSplitter->addWidget(panel);
        outputPanels.append(panel);
    }
}