# OCR & AI Pipeline — Vision Studio

> Detail plan for the image preprocessing pipeline and inference engine integration.
> Referenced from [MASTER_PLAN.md](MASTER_PLAN.md) §4.

---

## Preprocessing (ImageProcessor — OpenCV)

```
Input Image → Load (imread) → Grayscale (cvtColor) → Adaptive Threshold → [Optional: Perspective Correction] → Output
```

| Stage | OpenCV Function | Purpose |
|-------|----------------|---------|
| Grayscale | `cv::cvtColor(BGR2GRAY)` | Reduce to single channel |
| Threshold | `cv::adaptiveThreshold` | Binarize text vs background |
| Deskew | `cv::getPerspectiveTransform` + `warpPerspective` | Correct tilted documents |
| Blur | `cv::GaussianBlur` | Noise reduction before inference |

The `ImageProcessor` uses a **fluent/chaining API**:
```cpp
processor.load("doc.png")
         .toGrayscale()
         .applyThreshold(11, 2.0)
         .correctPerspective();
```

---

## Inference Engine

| Backend | Macro | Use Case |
|---------|-------|----------|
| ncnn | `VISIONCORE_HAS_NCNN` | Lightweight, mobile-friendly, no GPU required |
| ONNX Runtime | `VISIONCORE_HAS_ONNXRT` | Broader model compatibility, GPU acceleration |

Backend availability is detected at CMake configure time. VisionEngine selects the best available at runtime.

---

## UI Design (VisionApp — Qt 5)

### Window Layout

```
┌────────────────────────────────────────────────────────────────┐
│  Menu Bar: File | Process | View | Help                        │
├────────────────────────────────────────────────────────────────┤
│  Toolbar: [Open] | [Preprocess] [Run OCR]                      │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│                    Image Viewer (central)                      │
│                 Zoom / Pan / Fit to Window                     │
│                                                                │
├────────────────────────────────────────────────────────────────┤
│  Log Console (dockable)                                        │
│  [INFO]  VisionCore loaded successfully.                       │
│  [DEBUG] Preprocessing: grayscale + threshold (block=11, C=2)  │
│  [INFO]  OCR [0.95]: "Invoice #12345"                          │
└────────────────────────────────────────────────────────────────┘
│  Status Bar: "Loaded: sample.png"                              │
└────────────────────────────────────────────────────────────────┘
```

### Theme
- **Dark theme** via `dark_theme.qss` resource file.
- Loaded at startup in `main.cpp` from `:/styles/dark_theme.qss`.

### Key Shortcuts

| Action | Shortcut |
|--------|----------|
| Open Image | `Ctrl+O` |
| Preprocess | `Ctrl+P` |
| Run OCR | `Ctrl+R` |
| Quit | `Ctrl+Q` |
