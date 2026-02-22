// ParameterBox.h
#ifndef PARAMETERBOX_H
#define PARAMETERBOX_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class ParameterBox : public QWidget {
    Q_OBJECT
public:
    explicit ParameterBox(QWidget *parent = nullptr);
public slots:
    void updateParametersForTask(int taskIndex);
private:
    QHBoxLayout* layout;
    void clearLayout();
};
#endif // PARAMETERBOX_H