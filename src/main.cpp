#include <QApplication>
#include "frontend/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // --- The Advanced Artistic Theme with SVG Cube Pattern ---
    QString styleSheet = R"(
        QMainWindow {
            background-color: #F4F7F6;
            background-image: url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='60' height='103.92' viewBox='0 0 60 103.92'><g fill='%23e8ecf1'><path d='M30 17.32L60 0v34.64L30 51.96 0 34.64V0z'/></g><g fill='#dfe4ea'><path d='M30 51.96l30-17.32v34.64L30 86.6z'/></g><g fill='%23d5dbe2'><path d='M30 51.96L0 34.64v34.64l30 17.32z'/></g></svg>");
        }
        QWidget {
            color: #2D3436;
            font-family: 'Segoe UI', Helvetica, Arial, sans-serif;
            font-size: 13px;
        }
        TopTaskBar {
            background-color: rgba(255, 255, 255, 0.9);
            border-bottom: 2px solid #E0E6ED;
        }
        QPushButton {
            background-color: #FFFFFF;
            color: #2D3436;
            border: 1px solid #D1D8E0;
            border-radius: 6px;
            padding: 6px 16px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #E8F3F1; border-color: #00B894; color: #00B894; }
        QPushButton:pressed { background-color: #00B894; color: #FFFFFF; }
        QComboBox, QSpinBox {
            background-color: #FFFFFF;
            border: 1px solid #D1D8E0;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QLabel#imageDisplay {
            background-color: rgba(255, 255, 255, 0.85);
            border: 2px dashed #B2BEC3;
            border-radius: 8px;
        }
        QLabel#helpBubble {
            background-color: #0984e3;
            color: white;
            border-radius: 12px;
            font-weight: bold;
            font-size: 14px;
            min-width: 24px;
            min-height: 24px;
            max-width: 24px;
            max-height: 24px;
        }
        QLabel#helpBubble:hover {
            background-color: #74b9ff;
        }
        QToolTip {
            background-color: #2d3436;
            color: #dfe6e9;
            border: 1px solid #636e72;
            border-radius: 4px;
            padding: 6px;
            font-size: 13px;
        }
    )";
    app.setStyleSheet(styleSheet);
    
    MainWindow window;
    window.setWindowTitle("Advanced Computer Vision Studio");
    window.resize(1280, 800);
    window.show();
    
    return app.exec();
}