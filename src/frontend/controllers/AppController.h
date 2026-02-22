#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include "../components/ImagePanel.h"
#include "../components/TopTaskBar.h"
#include "ImageStateManager.h"

class AppController : public QObject {
    Q_OBJECT
public:
    AppController(ImagePanel* inputP, ImagePanel* outputP, TopTaskBar* tb, QObject *parent = nullptr);

private slots:
    void handleUpload();
    void handleApply();
    void handleClear();

private:
    ImagePanel* inputPanel;
    ImagePanel* outputPanel;
    TopTaskBar* taskBar;
    ImageStateManager stateManager;
};

#endif // APPCONTROLLER_H