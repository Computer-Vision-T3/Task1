#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QResizeEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <opencv2/opencv.hpp>

class ImagePanel : public QWidget {
    Q_OBJECT

public:
    explicit ImagePanel(const QString& title, bool isInput, QWidget *parent = nullptr);

    cv::Mat getImage() const;
    void displayImage(const cv::Mat& img);
    void clear();
    void setTitle(const QString& title);

signals:
    void imageLoaded(const cv::Mat& img);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void handleLoadImage();

private:
    // Layout
    QVBoxLayout* mainLayout;
    QWidget* headerWidget;
    QLabel* titleLabel;
    QLabel* badgeLabel;
    QLabel* imageSizeLabel;
    QLabel* imageDisplay;
    QPushButton* loadBtn;
    QWidget* overlayWidget;
    QLabel* overlayIcon;
    QLabel* overlayText;
    QLabel* overlaySubtext;
    QWidget* footerWidget;
    QLabel* channelInfo;
    QLabel* pixelCount;

    cv::Mat currentImage;
    QPixmap currentPixmap;
    bool isInput;
    QString titleText;

    QPixmap cvMatToPixmap(const cv::Mat& img);
    void updateDisplay();
    void updateFooterInfo();
    void setOverlayVisible(bool visible);
    QString formatImageSize(const cv::Mat& img);
    QString getChannelInfo(const cv::Mat& img);
};

#endif
