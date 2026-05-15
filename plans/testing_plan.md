# Testing Strategy — Vision Studio

> Detail plan for unit tests, regression tests, and test data conventions.
> Referenced from [MASTER_PLAN.md](MASTER_PLAN.md) §5.

---

## Unit Tests (GTest)

```
tests/test_image_processor.cpp
```
Tests the `ImageProcessor` class independently:
- Image loading from disk
- Grayscale conversion correctness
- Threshold output validation
- Perspective correction on known test images

**Run**: `ctest --test-dir build/windows-x64 --build-config Debug`

---

## Regression Tests (Python)

```
tests/regression_test.py
```
Compares OCR output against ground-truth:
- Input: `tests/data/sample1.png`
- Expected: `tests/data/sample1.expected.txt`
- Metrics: Exact match %, character-level accuracy (via `difflib.SequenceMatcher`)

**Run**: `python tests/regression_test.py -e bin/Release/VisionStudio.exe -d tests/data`

---

## Test Data Convention

```
tests/data/
    sample1.png              ← Input image
    sample1.expected.txt     ← Ground truth OCR text (UTF-8)
```

Every test image must have a corresponding `.expected.txt` file with the expected OCR output.
