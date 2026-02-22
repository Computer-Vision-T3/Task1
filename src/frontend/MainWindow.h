#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "components/ImagePanel.h"
#include "components/TopTaskBar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class AppController; // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // The magical function that re-draws the screen
    void rebuildPanels(int numInputs, int numOutputs, QStringList inTitles, QStringList outTitles);
    
    QList<ImagePanel*> inputPanels;
    QList<ImagePanel*> outputPanels;
    TopTaskBar* taskBar;

private:
    Ui::MainWindow *ui;
    AppController* appController;
    
    QSplitter* mainSplitter;
    QSplitter* leftSplitter;
    QSplitter* rightSplitter;
    
    void setupCustomLayout();
};

#endif // MAINWINDOW_H