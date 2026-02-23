#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QList>
#include "components/TopTaskBar.h"
#include "components/ImagePanel.h"

class AppController; // Forward declaration

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // GETTERS for the AppController
    TopTaskBar* getTopTaskBar() { return taskBar; }
    QList<ImagePanel*>& getInputPanels() { return inputPanels; }
    QList<ImagePanel*>& getOutputPanels() { return outputPanels; }

    // Logic to change UI based on task
    void updateLayoutForTask(int taskIndex);

private:
    Ui::MainWindow *ui;
    AppController* appController;
    
    TopTaskBar* taskBar;
    QSplitter* mainSplitter;
    QSplitter* leftSplitter;
    QSplitter* rightSplitter;

    QList<ImagePanel*> inputPanels;
    QList<ImagePanel*> outputPanels;

    void setupCustomLayout();
    void rebuildPanels(int numInputs, int numOutputs, QStringList inTitles, QStringList outTitles);
};

#endif
