#ifndef TOPTASKBAR_H
#define TOPTASKBAR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "ParameterBox.h"

class TopTaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TopTaskBar(QWidget *parent = nullptr);
    int getSelectedOperation() const;
    ParameterBox* getParameterBox() const { return paramBox; }

signals:
    void applyRequested();
    void saveRequested();
    void clearRequested();
    void taskChanged(int taskIndex);

private:
    QComboBox* operationSelector;
    QPushButton* applyBtn;
    QPushButton* saveBtn;
    QPushButton* clearBtn;
    ParameterBox* paramBox;
    QLabel* helpBubble; // The new dynamic help bubble

    void updateHelpText(int taskIndex);
};

#endif // TOPTASKBAR_H