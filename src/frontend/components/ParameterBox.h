#ifndef PARAMETERBOX_H
#define PARAMETERBOX_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>

class ParameterBox : public QWidget {
    Q_OBJECT

public:
    explicit ParameterBox(QWidget *parent = nullptr);

public slots:
    // This receives the signal when the user changes the task dropdown
    void updateParametersForTask(int taskIndex);

private:
    QHBoxLayout* layout;
    
    // Helper to wipe the old parameters off the screen
    void clearLayout();
};

#endif // PARAMETERBOX_H