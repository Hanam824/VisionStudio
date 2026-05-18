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

void ImageViewer::setImageFromData(const uint8_t* imageData, int width, int height, int channels) {
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

    // Create QImage (non-owning). Note: OpenCV step may differ from width*channels.
    QImage img(imageData, width, height, width * channels, fmt);

    if (channels == 3) {
        // BGR → RGB swap.
        img = img.rgbSwapped();
    }

    setImage(img);
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
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = event->pos();
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
        QPoint delta = event->pos() - m_panStart;
        m_panStart = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}
