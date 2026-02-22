#include <QApplication>
#include "frontend/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.setWindowTitle("Computer Vision Task 1");
    window.resize(1200, 700);
    window.show();
    
    return app.exec();
}