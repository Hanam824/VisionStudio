#pragma once

#include "VisionCore/IVisionEngine.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QImage>

#include <vector>

// ── ImageViewer ─────────────────────────────────────────────────────────────
/**
 * @brief Widget for displaying and interacting with images.
 *
 * Supports zoom (scroll wheel), pan (middle-click drag), and OCR box overlays.
 */
class ImageViewer : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);
    ~ImageViewer() override = default;

    /**
     * @brief Display a QImage in the viewer.
     * @param image Image to display.
     */
    void setImage(const QImage& image);

    /**
     * @brief Display raw BGR pixel data (from VisionCore).
     * @param imageData Pointer to raw pixel buffer.
     * @param width     Image width in pixels.
     * @param height    Image height in pixels.
     * @param channels  Number of channels (e.g. 3 for BGR).
     * @param step      Row stride in bytes.
     */
    void setImageFromData(const uint8_t* imageData, int width, int height, int channels, int step);

    /**
     * @brief Display OCR bounding boxes overlaid on the image.
     * @param results Detected OCR text regions to overlay.
     */
    void setOcrResults(const std::vector<vision::OcrResult>& results);

    /**
     * @brief Clear all OCR bounding boxes.
     */
    void clearOcrResults();

    /**
     * @brief Highlight a specific OCR bounding box index.
     * @param index Zero-based index into the current OCR results.
     */
    void highlightOcrIndex(int index);

    /**
     * @brief Fit the image to the viewport.
     */
    void fitToView();

    /**
     * @brief Reset zoom to 100%.
     */
    void resetZoom();

signals:
    /**
     * @brief Emitted when the user scrolls to zoom.
     * @param factor New zoom factor.
     */
    void zoomChanged(double factor);

    /**
     * @brief Emitted when the user clicks an OCR bounding box overlay.
     * @param index Zero-based index of the clicked OCR box.
     */
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
