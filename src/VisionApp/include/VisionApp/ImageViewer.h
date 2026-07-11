#pragma once

#include "VisionCore/IVisionEngine.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QImage>

#include <vector>

// ── ImageViewer ─────────────────────────────────────────────────────────────
/// Widget for displaying and interacting with images.
/// Supports zoom (scroll wheel), pan (middle-click drag), and OCR box overlays.
class ImageViewer : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);
    ~ImageViewer() override = default;

    /// Display a QImage in the viewer.
    void setImage(const QImage& image);

    /// Display raw BGR pixel data (from VisionCore).
    void setImageFromData(const uint8_t* imageData, int width, int height, int channels, int step);

    /// Display OCR bounding boxes overlaid on the image.
    void setOcrResults(const std::vector<vision::OcrResult>& results);

    /// Clear all OCR bounding boxes.
    void clearOcrResults();

    /// Highlight a specific OCR bounding box index.
    void highlightOcrIndex(int index);

    /// Fit the image to the viewport.
    void fitToView();

    /// Reset zoom to 100%.
    void resetZoom();

signals:
    /// Emitted when the user scrolls to zoom.
    void zoomChanged(double factor);

    /// Emitted when the user clicks an OCR bounding box overlay.
    void ocrBoxClicked(int index);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QGraphicsScene*                   m_scene        = nullptr;
    QGraphicsPixmapItem*              m_pixmapItem   = nullptr;
    std::vector<QGraphicsRectItem*>   m_ocrBoxItems;
    double                            m_zoomFactor   = 1.0;
    bool                              m_panning      = false;
    QPoint                            m_panStart;
};
