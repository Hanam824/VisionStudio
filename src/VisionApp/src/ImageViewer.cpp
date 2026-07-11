#include "VisionApp/ImageViewer.h"

#include <QScrollBar>
#include <QWheelEvent>
#include <QMouseEvent>

#include <algorithm>
#include <cmath>

// ── Construction ────────────────────────────────────────────────────────────

ImageViewer::ImageViewer(QWidget* parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pixmapItem = m_scene->addPixmap(QPixmap());
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);

    // View settings.
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    // Dark background.
    setBackgroundBrush(QColor(30, 30, 30));

    // Remove border.
    setFrameShape(QFrame::NoFrame);
}

// ── Image Display ───────────────────────────────────────────────────────────

void ImageViewer::setImage(const QImage& image) {
    m_pixmapItem->setPixmap(QPixmap::fromImage(image));
    m_scene->setSceneRect(m_pixmapItem->boundingRect());
    fitToView();
}

void ImageViewer::setImageFromData(const uint8_t* imageData, int width, int height, int channels, int step) {
    if (!imageData || width <= 0 || height <= 0) return;

    QImage::Format fmt;
    if (channels == 1) {
        fmt = QImage::Format_Grayscale8;
    } else if (channels == 3) {
        // OpenCV BGR → Qt expects RGB. We'll convert below.
        fmt = QImage::Format_RGB888;
    } else if (channels == 4) {
        fmt = QImage::Format_RGBA8888;
    } else {
        return; // Unsupported format.
    }

    // Create QImage (non-owning).
    QImage img(imageData, width, height, step, fmt);

    if (channels == 3) {
        // BGR → RGB swap.
        img = img.rgbSwapped();
    }

    setImage(img);
}

void ImageViewer::clearOcrResults() {
    for (auto* item : m_ocrBoxItems) {
        if (item && m_scene) {
            m_scene->removeItem(item);
            delete item;
        }
    }
    m_ocrBoxItems.clear();
}

void ImageViewer::setOcrResults(const std::vector<vision::OcrResult>& results) {
    clearOcrResults();

    QPen pen(QColor(0, 220, 255, 220), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush brush(QColor(0, 220, 255, 30));

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        auto* rectItem = m_scene->addRect(QRectF(r.x, r.y, r.w, r.h), pen, brush);
        rectItem->setZValue(10.0);
        rectItem->setToolTip(QString("OCR #%1 [%2%]: %3")
                             .arg(i + 1)
                             .arg(static_cast<int>(r.confidence * 100))
                             .arg(QString::fromStdString(r.text)));
        rectItem->setData(0, static_cast<int>(i));
        m_ocrBoxItems.push_back(rectItem);
    }
}

void ImageViewer::highlightOcrIndex(int index) {
    for (size_t i = 0; i < m_ocrBoxItems.size(); ++i) {
        auto* item = m_ocrBoxItems[i];
        if (!item) continue;
        if (static_cast<int>(i) == index) {
            QPen highlightPen(QColor(255, 160, 0, 255), 3);
            QBrush highlightBrush(QColor(255, 160, 0, 80));
            item->setPen(highlightPen);
            item->setBrush(highlightBrush);
            item->setZValue(20.0);
        } else {
            QPen normalPen(QColor(0, 220, 255, 220), 2);
            QBrush normalBrush(QColor(0, 220, 255, 30));
            item->setPen(normalPen);
            item->setBrush(normalBrush);
            item->setZValue(10.0);
        }
    }
}

void ImageViewer::fitToView() {
    if (m_pixmapItem->pixmap().isNull()) return;
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    m_zoomFactor = transform().m11();
}

void ImageViewer::resetZoom() {
    resetTransform();
    m_zoomFactor = 1.0;
    emit zoomChanged(m_zoomFactor);
}

// ── Mouse / Wheel Events ────────────────────────────────────────────────────

void ImageViewer::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
        m_zoomFactor *= scaleFactor;
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        m_zoomFactor /= scaleFactor;
    }
    emit zoomChanged(m_zoomFactor);
}

void ImageViewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->position().toPoint());
        for (size_t i = 0; i < m_ocrBoxItems.size(); ++i) {
            auto* item = m_ocrBoxItems[i];
            if (item && item->rect().contains(scenePos)) {
                highlightOcrIndex(static_cast<int>(i));
                emit ocrBoxClicked(static_cast<int>(i));
                event->accept();
                return;
            }
        }
    }

    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = event->position().toPoint();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent* event) {
    if (m_panning) {
        QPoint delta = event->position().toPoint() - m_panStart;
        m_panStart = event->position().toPoint();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}
