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

signals:
    void uploadRequested();
    void applyRequested();
    void clearRequested();

private:
    QComboBox* operationSelector;
    QPushButton* uploadBtn;
    QPushButton* applyBtn;
    QPushButton* clearBtn;
    ParameterBox* paramBox;
};

#endif // TOPTASKBAR_H