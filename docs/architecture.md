# Architecture — Vision Studio

> System design overview for contributors and maintainers.

---

## Two-Module Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        VisionApp (Executable)                       │
│  Qt 5 Widgets · Dark Theme · Toolbar · Image Viewer · Log Console  │
│                                                                     │
│  Loads VisionCore at runtime via QLibrary / dlopen                  │
│  Communicates ONLY through IVisionEngine abstract interface         │
└───────────────┬─────────────────────────────────────────────────────┘
                │  createVisionEngine()  (C-linkage factory)
                ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    VisionCore (Shared Library / DLL)                 │
│  IVisionEngine (pure virtual) ← VisionEngine (concrete impl)       │
│  ImageProcessor (OpenCV pipeline)                                   │
│  Inference Engine (ncnn / ONNX Runtime)                             │
└─────────────────────────────────────────────────────────────────────┘
```

## Why This Design?

1. **LGPLv3 Compliance** — Qt 5 is LGPL. Keeping AI/image code in a separate DLL (`VisionCore`) and communicating via abstract interfaces means VisionApp links dynamically, so neither module imposes licensing constraints on the other.

2. **Plugin Architecture** — Third parties can provide alternative `IVisionEngine` implementations without modifying VisionApp.

3. **Independent Testing** — VisionCore compiles and tests (GTest) without the Qt UI.

---

## Key Interfaces

### IVisionEngine (Abstract)

The sole communication contract between VisionApp and VisionCore:

- `initialize()` / `shutdown()` — lifecycle management
- `loadImage(path)` — load an image from disk
- `getImageData(w, h, ch)` — retrieve raw pixel buffer (BGR format)
- `preprocess(opts)` — run OpenCV pipeline (grayscale, threshold, deskew)
- `runOcr()` → `vector<OcrResult>` — run inference
- `setLogCallback(fn)` — register a callback for log messages

### Dynamic Loading

VisionApp loads VisionCore at runtime using `QLibrary::resolve("createVisionEngine")`. This C-linkage factory returns a new `IVisionEngine*` that the app wraps in `std::unique_ptr`.

### Log Callback

VisionCore emits log messages via a `LogCallback`. VisionApp routes these through `QMetaObject::invokeMethod` for thread-safe UI updates in the `LogConsole` widget.

---

## Data Flow

```
User opens image → MainWindow::onOpenImage()
    → engine->loadImage(path)
        → ImageProcessor::load() [OpenCV imread]
    → engine->getImageData() → ImageViewer::setImageFromData()
        → BGR→RGB conversion → QGraphicsView display

User clicks Preprocess → MainWindow::onPreprocess()
    → engine->preprocess(opts)
        → ImageProcessor::toGrayscale() → applyThreshold()
    → updateImageDisplay()

User clicks Run OCR → MainWindow::onRunOcr()
    → engine->runOcr()
        → [ncnn/ONNX inference — placeholder]
    → results displayed in LogConsole
```

---

## Module Boundaries

| Module | Knows About | Does NOT Know About |
|--------|-------------|---------------------|
| **VisionApp** | `IVisionEngine.h`, `Export.h` (public headers) | `VisionEngine.h`, `ImageProcessor.h` (internals) |
| **VisionCore** | OpenCV, ncnn, ONNX Runtime | Qt, UI widgets, QSS themes |

> **Rule**: VisionApp must NEVER `#include` VisionCore internals. Only `IVisionEngine.h` + `Export.h`.

---

## Thread Safety

- `VisionEngine` uses `std::mutex` for all public methods and `std::atomic_bool` for the ready state.
- The `LogCallback` can be invoked from any thread; VisionApp uses `QMetaObject::invokeMethod` for thread-safe UI updates.
