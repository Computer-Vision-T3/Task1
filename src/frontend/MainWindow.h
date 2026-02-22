#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "components/ImagePanel.h"
#include "components/TopTaskBar.h"
#include "controllers/AppController.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    
    TopTaskBar* taskBar;
    ImagePanel* inputPanel;
    ImagePanel* outputPanel;
    AppController* appController;
    
    void setupCustomLayout();
};

#endif // MAINWINDOW_H