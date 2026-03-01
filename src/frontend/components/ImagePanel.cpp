#include "ImagePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QImage>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QStackedWidget>
#include <QPainter>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

ImagePanel::ImagePanel(const QString& title, bool isInput, QWidget *parent)
    : QWidget(parent), isInput(isInput), titleText(title)
{
    setObjectName("imagePanel");
    setAcceptDrops(true);
    setMinimumSize(200, 180);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Header ──────────────────────────────────────────
    headerWidget = new QWidget(this);
    headerWidget->setObjectName("imagePanelHeader");
    headerWidget->setFixedHeight(42);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(14, 0, 10, 0);
    headerLayout->setSpacing(8);

    // Left: Icon dot + title
    QLabel* dotIndicator = new QLabel(this);
    dotIndicator->setFixedSize(8, 8);
    if (isInput) {
        dotIndicator->setStyleSheet("background-color: #2D9B6F; border-radius: 4px;");
    } else {
        dotIndicator->setStyleSheet("background-color: #5B4FCF; border-radius: 4px;");
    }

    titleLabel = new QLabel(title.toUpper(), this);
    titleLabel->setObjectName("panelTitle");

    badgeLabel = new QLabel(isInput ? "INPUT" : "OUTPUT", this);
    badgeLabel->setObjectName(isInput ? "panelBadgeInput" : "panelBadge");

    imageSizeLabel = new QLabel(this);
    imageSizeLabel->setStyleSheet("font-size: 10px; color: #C4BDB4; font-weight: 500;");
    imageSizeLabel->hide();

    headerLayout->addWidget(dotIndicator);
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(badgeLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(imageSizeLabel);

    if (isInput) {
        // Small clear button in header for input panels
        QPushButton* clearMini = new QPushButton("✕", this);
        clearMini->setObjectName("iconBtn");
        clearMini->setFixedSize(22, 22);
        clearMini->setToolTip("Clear image");
        clearMini->setCursor(Qt::PointingHandCursor);
        connect(clearMini, &QPushButton::clicked, this, &ImagePanel::clear);
        headerLayout->addWidget(clearMini);
    }

    mainLayout->addWidget(headerWidget);

    // ── Thin separator ──────────────────────────────────
    QFrame* sep = new QFrame(this);
    sep->setObjectName("divider");
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    mainLayout->addWidget(sep);

    // ── Canvas (image display + overlay) ────────────────
    QWidget* canvasContainer = new QWidget(this);
    canvasContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout* canvasLayout = new QVBoxLayout(canvasContainer);
    canvasLayout->setContentsMargins(12, 12, 12, 8);
    canvasLayout->setSpacing(0);

    imageDisplay = new QLabel(this);
    imageDisplay->setObjectName("imageDisplay");
    imageDisplay->setAlignment(Qt::AlignCenter);
    imageDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageDisplay->setMinimumHeight(120);

    // Build placeholder content
    if (isInput) {
        imageDisplay->setText("");
        // Overlay widget for empty state
        overlayWidget = new QWidget(imageDisplay);
        overlayWidget->setStyleSheet("background-color: transparent;");
        QVBoxLayout* ovl = new QVBoxLayout(overlayWidget);
        ovl->setAlignment(Qt::AlignCenter);
        ovl->setSpacing(6);

        overlayIcon = new QLabel("⊕", overlayWidget);
        overlayIcon->setAlignment(Qt::AlignCenter);
        overlayIcon->setStyleSheet("font-size: 28px; color: #D4CEC6;");

        overlayText = new QLabel("Drop image here", overlayWidget);
        overlayText->setAlignment(Qt::AlignCenter);
        overlayText->setStyleSheet("font-size: 13px; font-weight: 600; color: #B8B0A6;");

        overlaySubtext = new QLabel("or click to browse", overlayWidget);
        overlaySubtext->setAlignment(Qt::AlignCenter);
        overlaySubtext->setStyleSheet("font-size: 11px; color: #C8C2BA;");

        ovl->addWidget(overlayIcon);
        ovl->addWidget(overlayText);
        ovl->addWidget(overlaySubtext);
        overlayWidget->setLayout(ovl);
    }

    canvasLayout->addWidget(imageDisplay, 1);
    mainLayout->addWidget(canvasContainer, 1);

    // ── Footer with metadata ─────────────────────────────
    footerWidget = new QWidget(this);
    footerWidget->setFixedHeight(32);
    footerWidget->setStyleSheet(
        "background-color: #FAFAF8;"
        "border-top: 1px solid #F0ECE6;"
        "border-radius: 0 0 16px 16px;"
    );
    QHBoxLayout* footerLayout = new QHBoxLayout(footerWidget);
    footerLayout->setContentsMargins(14, 0, 14, 0);
    footerLayout->setSpacing(12);

    channelInfo = new QLabel(this);
    channelInfo->setStyleSheet("font-size: 10px; color: #C4BDB4; font-weight: 500;");

    pixelCount = new QLabel(this);
    pixelCount->setStyleSheet("font-size: 10px; color: #C4BDB4; font-weight: 500;");

    if (isInput) {
        loadBtn = new QPushButton("⊕  Load Image", this);
        loadBtn->setObjectName("loadBtn");
        loadBtn->setCursor(Qt::PointingHandCursor);
        loadBtn->setFixedHeight(24);
        connect(loadBtn, &QPushButton::clicked, this, &ImagePanel::handleLoadImage);
        footerLayout->addWidget(loadBtn);
    }

    footerLayout->addStretch();
    footerLayout->addWidget(channelInfo);
    footerLayout->addWidget(pixelCount);
    mainLayout->addWidget(footerWidget);

    // Initial state
    if (isInput && overlayWidget) {
        QTimer::singleShot(0, [this]() {
            overlayWidget->setGeometry(imageDisplay->rect());
        });
    }
}

cv::Mat ImagePanel::getImage() const { return currentImage; }

void ImagePanel::setTitle(const QString& title) {
    titleText = title;
    titleLabel->setText(title.toUpper());
}

void ImagePanel::displayImage(const cv::Mat& img) {
    if (img.empty()) return;
    currentImage = img.clone();
    currentPixmap = cvMatToPixmap(currentImage);

    imageDisplay->setStyleSheet(
        "background-color: #F0EEF9;"
        "border-radius: 10px;"
    );

    if (isInput && overlayWidget) {
        overlayWidget->hide();
    }

    if (isInput && loadBtn) {
        loadBtn->setText("⊕  Replace");
    }

    imageSizeLabel->setText(formatImageSize(img));
    imageSizeLabel->show();

    updateFooterInfo();
    updateDisplay();
    emit imageLoaded(currentImage);
}

void ImagePanel::updateDisplay() {
    if (!currentPixmap.isNull() && imageDisplay->width() > 10 && imageDisplay->height() > 10) {
        QSize targetSize = imageDisplay->size() - QSize(8, 8);
        imageDisplay->setPixmap(
            currentPixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    }
}

void ImagePanel::updateFooterInfo() {
    if (currentImage.empty()) return;
    channelInfo->setText(getChannelInfo(currentImage));
    long total = (long)currentImage.rows * currentImage.cols;
    if (total >= 1000000) {
        pixelCount->setText(QString("%1 MP").arg(total / 1000000.0, 0, 'f', 1));
    } else if (total >= 1000) {
        pixelCount->setText(QString("%1K px").arg(total / 1000));
    } else {
        pixelCount->setText(QString("%1 px").arg(total));
    }
}

QString ImagePanel::formatImageSize(const cv::Mat& img) {
    return QString("%1 × %2").arg(img.cols).arg(img.rows);
}

QString ImagePanel::getChannelInfo(const cv::Mat& img) {
    if (img.channels() == 1) return "Grayscale";
    if (img.channels() == 3) return "RGB";
    if (img.channels() == 4) return "RGBA";
    return QString("%1ch").arg(img.channels());
}

void ImagePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateDisplay();
    if (isInput && overlayWidget && currentImage.empty()) {
        overlayWidget->setGeometry(imageDisplay->rect());
    }
}

void ImagePanel::dragEnterEvent(QDragEnterEvent* event) {
    if (isInput && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        imageDisplay->setStyleSheet(
            "background-color: #EDE8FF;"
            "border: 2px solid #5B4FCF;"
            "border-radius: 10px;"
        );
    }
}

void ImagePanel::dropEvent(QDropEvent* event) {
    if (!isInput) return;
    QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString path = urls.first().toLocalFile();
        cv::Mat img = cv::imread(path.toStdString());
        if (!img.empty()) displayImage(img);
    }
    if (currentImage.empty()) {
        imageDisplay->setStyleSheet(
            "background-color: #F9F7F4;"
            "border-radius: 10px;"
        );
    }
}

void ImagePanel::enterEvent(QEvent* event) {
    QWidget::enterEvent(event);
}

void ImagePanel::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
}

void ImagePanel::clear() {
    currentImage = cv::Mat();
    currentPixmap = QPixmap();
    imageDisplay->clear();
    imageDisplay->setStyleSheet(
        "background-color: #F9F7F4;"
        "border-radius: 10px;"
    );
    imageSizeLabel->hide();
    channelInfo->setText("");
    pixelCount->setText("");

    if (isInput) {
        if (overlayWidget) {
            overlayWidget->setGeometry(imageDisplay->rect());
            overlayWidget->show();
        }
        if (loadBtn) loadBtn->setText("⊕  Load Image");
    }
}

void ImagePanel::handleLoadImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open Image", "",
        "Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif *.webp);;All Files (*)"
    );
    if (!fileName.isEmpty()) {
        cv::Mat img = cv::imread(fileName.toStdString());
        if (!img.empty()) displayImage(img);
    }
}

QPixmap ImagePanel::cvMatToPixmap(const cv::Mat& img) {
    if (img.empty()) return QPixmap();
    cv::Mat temp;
    if (img.channels() == 3) {
        cv::cvtColor(img, temp, cv::COLOR_BGR2RGB);
        QImage qimg((const uchar*)temp.data, temp.cols, temp.rows, (int)temp.step, QImage::Format_RGB888);
        return QPixmap::fromImage(qimg.copy());
    } else if (img.channels() == 1) {
        QImage qimg((const uchar*)img.data, img.cols, img.rows, (int)img.step, QImage::Format_Grayscale8);
        return QPixmap::fromImage(qimg.copy());
    } else if (img.channels() == 4) {
        cv::cvtColor(img, temp, cv::COLOR_BGRA2RGBA);
        QImage qimg((const uchar*)temp.data, temp.cols, temp.rows, (int)temp.step, QImage::Format_RGBA8888);
        return QPixmap::fromImage(qimg.copy());
    }
    return QPixmap();
}