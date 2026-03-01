#ifndef PARAMETERBOX_H
#define PARAMETERBOX_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QFrame>

class ParameterBox : public QWidget {
    Q_OBJECT

public:
    explicit ParameterBox(QWidget *parent = nullptr);

public slots:
    void updateParametersForTask(int taskIndex);

private:
    QHBoxLayout* layout;
    void clearLayout();

    // Helper builders
    QWidget* buildLabeledCombo(const QString& label, const QString& objName,
                                const QStringList& items, const QString& tooltip = "");
    QWidget* buildLabeledSlider(const QString& label, const QString& objName,
                                 int min, int max, int value);
    QWidget* buildLabeledSpin(const QString& label, const QString& objName,
                               int min, int max, int step, int value);
    QFrame* buildSeparator();
};

#endif // PARAMETERBOX_H
