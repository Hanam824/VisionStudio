# Architecture — Vision Studio

> Detail plan for the two-module design, interface contract, and dynamic loading pattern.
> Referenced from [MASTER_PLAN.md](MASTER_PLAN.md) §2.

---

## Two-Module Design

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

## Why This Separation Matters

1. **LGPLv3 Compliance**: Qt 5 is licensed under LGPLv3. By keeping AI/image logic in a separate DLL (`VisionCore`) and communicating only through abstract interfaces, VisionApp can link dynamically without imposing LGPL on proprietary code.

2. **Plugin Architecture**: Third parties can provide alternative `IVisionEngine` implementations without modifying VisionApp.

3. **Testability**: VisionCore can be tested independently (GTest) without spinning up the Qt UI.

---

## IVisionEngine — Abstract Interface

```cpp
// src/VisionCore/include/VisionCore/IVisionEngine.h
namespace vision {

class VISIONCORE_API IVisionEngine {
public:
    virtual ~IVisionEngine() = default;

    // Lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    [[nodiscard]] virtual bool isReady() const = 0;

    // Image I/O
    virtual bool loadImage(const std::string& filePath) = 0;
    [[nodiscard]] virtual const uint8_t* getImageData(
        int& width, int& height, int& channels) const = 0;

    // Preprocessing (OpenCV)
    virtual bool preprocess(const PreprocessOptions& opts = {}) = 0;

    // OCR / Inference
    virtual std::vector<OcrResult> runOcr() = 0;

    // Logging (callback to UI)
    virtual void setLogCallback(LogCallback callback) = 0;

    // Info
    [[nodiscard]] virtual std::string versionString() const = 0;
};

} // namespace vision

// C-linkage factory for dynamic loading
VISIONCORE_C_API vision::IVisionEngine* createVisionEngine();
```

---

## DLL Export Macros

```cpp
// src/VisionCore/include/VisionCore/Export.h
#if defined(_WIN32) || defined(_WIN64)
    #ifdef VISIONCORE_EXPORTS
        #define VISIONCORE_API __declspec(dllexport)
    #else
        #define VISIONCORE_API __declspec(dllimport)
    #endif
#elif defined(__APPLE__) || defined(__linux__)
    #ifdef VISIONCORE_EXPORTS
        #define VISIONCORE_API __attribute__((visibility("default")))
    #else
        #define VISIONCORE_API
    #endif
#else
    #define VISIONCORE_API
#endif

#define VISIONCORE_C_API extern "C" VISIONCORE_API
```

---

## Dynamic Loading Pattern (VisionApp)

```cpp
// In MainWindow::loadVisionCore()
QLibrary coreLib("VisionCore");
if (!coreLib.load()) { /* handle error */ }

auto createFunc = reinterpret_cast<CreateEngineFunc>(
    coreLib.resolve("createVisionEngine"));

std::unique_ptr<vision::IVisionEngine> engine(createFunc());
engine->setLogCallback([this](vision::LogLevel lv, const std::string& msg) {
    QMetaObject::invokeMethod(m_logConsole, [=]() {
        m_logConsole->appendMessage(static_cast<int>(lv),
                                    QString::fromStdString(msg));
    });
});
engine->initialize();
```

---

## OCR Result Structure

```cpp
struct OcrResult {
    std::string text;          // Recognized text
    float       confidence;    // Score [0.0, 1.0]
    int         x, y, w, h;   // Bounding box (image coords)
};
```

## Logging System

The engine communicates with the UI through a `LogCallback`:
```
VisionCore (any thread)  →  LogCallback  →  QMetaObject::invokeMethod  →  LogConsole (UI thread)
```

Log levels: `Debug` · `Info` · `Warning` · `Error` — each displayed with distinct colors in the console.
