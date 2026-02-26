#ifndef TOPTASKBAR_H
#define TOPTASKBAR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
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
    void taskChanged(int taskIndex); // <-- This is the missing piece!

private:
    QComboBox* operationSelector;
    QPushButton* applyBtn;
    QPushButton* saveBtn;
    QPushButton* clearBtn;
    ParameterBox* paramBox;
};

#endif // TOPTASKBAR_H