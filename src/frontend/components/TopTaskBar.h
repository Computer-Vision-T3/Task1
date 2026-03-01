#ifndef TOPTASKBAR_H
#define TOPTASKBAR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QSizePolicy>
#include "ParameterBox.h"

class TopTaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TopTaskBar(QWidget *parent = nullptr);
    int getSelectedOperation() const;
    ParameterBox* getParameterBox() const { return paramBox; }
    void setStatus(const QString& msg, bool isSuccess = true);
    void setProcessing(bool processing);

signals:
    void applyRequested();
    void saveRequested();
    void clearRequested();
    void taskChanged(int taskIndex);

private:
    // Row 1
    QComboBox*    operationSelector;
    QLabel*       helpBubble;
    ParameterBox* paramBox;

    // Row 2
    QLabel*       taskDescLabel;
    QLabel*       statusLabel;
    QPushButton*  clearBtn;
    QPushButton*  saveBtn;
    QPushButton*  applyBtn;

    QTimer*       statusTimer;

    void updateHelpText(int taskIndex);
};

#endif // TOPTASKBAR_H