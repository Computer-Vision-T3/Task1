#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setupCustomLayout();
    
    // Initialize controller and connect UI to logic
    appController = new AppController(inputPanel, outputPanel, taskBar, this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupCustomLayout() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    taskBar = new TopTaskBar(this);
    mainLayout->addWidget(taskBar);
    
    QHBoxLayout* imageLayout = new QHBoxLayout();
    inputPanel = new ImagePanel("Input", this);
    outputPanel = new ImagePanel("Output", this);
    
    imageLayout->addWidget(inputPanel);
    imageLayout->addWidget(outputPanel);
    
    mainLayout->addLayout(imageLayout);
    setCentralWidget(centralWidget);
}