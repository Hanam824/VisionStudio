#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>

// ── ImageViewer ─────────────────────────────────────────────────────────────
/// Widget for displaying and interacting with images.
/// Supports zoom (scroll wheel) and pan (middle-click drag).
class ImageViewer : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageViewer(QWidget* parent = nullptr);
    ~ImageViewer() override = default;

    /// Display a QImage in the viewer.
    void setImage(const QImage& image);

    /// Display raw BGR pixel data (from VisionCore).
    void setImageFromData(const uint8_t* data, int width, int height, int channels);

    /// Fit the image to the viewport.
    void fitToView();

    /// Reset zoom to 100%.
    void resetZoom();

signals:
    /// Emitted when the user scrolls to zoom.
    void zoomChanged(double factor);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QGraphicsScene*      m_scene      = nullptr;
    QGraphicsPixmapItem* m_pixmapItem = nullptr;
    double               m_zoomFactor = 1.0;
    bool                 m_panning    = false;
    QPoint               m_panStart;
};
