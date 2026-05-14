#!/usr/bin/env python3
"""
Vision Studio — OCR Regression Test Script

Compares OCR output from VisionStudio against ground-truth text files.

Usage:
    python regression_test.py --executable <path/to/VisionStudio> --data <path/to/tests/data>

Expected directory layout:
    tests/data/
        sample1.png          # Input image
        sample1.expected.txt  # Expected OCR output (ground truth)
        sample2.jpg
        sample2.expected.txt
        ...

The script will:
  1. Run VisionStudio CLI (when available) on each image.
  2. Compare output text against the .expected.txt file.
  3. Report accuracy metrics (exact match %, character-level accuracy).
"""

import argparse
import os
import sys
import subprocess
import difflib
from pathlib import Path


def find_test_pairs(data_dir: Path) -> list:
    """Find image files that have corresponding .expected.txt files."""
    image_exts = {'.png', '.jpg', '.jpeg', '.bmp', '.tiff', '.tif'}
    pairs = []

    for img_path in sorted(data_dir.iterdir()):
        if img_path.suffix.lower() in image_exts:
            expected = img_path.with_suffix('.expected.txt')
            if expected.exists():
                pairs.append((img_path, expected))

    return pairs


def run_ocr(executable: Path, image_path: Path) -> str:
    """Run the VisionStudio executable on an image and capture OCR output."""
    try:
        result = subprocess.run(
            [str(executable), '--ocr', str(image_path)],
            capture_output=True,
            text=True,
            timeout=30
        )
        return result.stdout.strip()
    except subprocess.TimeoutExpired:
        return "[TIMEOUT]"
    except FileNotFoundError:
        return "[EXECUTABLE NOT FOUND]"


def character_accuracy(expected: str, actual: str) -> float:
    """Compute character-level accuracy using SequenceMatcher."""
    if not expected and not actual:
        return 1.0
    if not expected or not actual:
        return 0.0
    matcher = difflib.SequenceMatcher(None, expected, actual)
    return matcher.ratio()


def main():
    parser = argparse.ArgumentParser(description='VisionStudio OCR Regression Test')
    parser.add_argument('--executable', '-e', type=Path, required=True,
                        help='Path to VisionStudio executable')
    parser.add_argument('--data', '-d', type=Path, default=Path('tests/data'),
                        help='Path to test data directory')
    args = parser.parse_args()

    if not args.data.exists():
        print(f"Error: Data directory '{args.data}' does not exist.", file=sys.stderr)
        sys.exit(1)

    pairs = find_test_pairs(args.data)
    if not pairs:
        print("No test pairs found. Ensure .expected.txt files exist alongside images.")
        sys.exit(0)

    print(f"Found {len(pairs)} test pair(s).\n")

    exact_matches = 0
    total_char_accuracy = 0.0

    for img_path, expected_path in pairs:
        expected_text = expected_path.read_text(encoding='utf-8').strip()
        actual_text = run_ocr(args.executable, img_path)

        exact = expected_text == actual_text
        char_acc = character_accuracy(expected_text, actual_text)

        status = "PASS" if exact else "FAIL"
        print(f"[{status}] {img_path.name}")
        print(f"  Char accuracy: {char_acc:.1%}")

        if not exact:
            print(f"  Expected: {expected_text[:80]}...")
            print(f"  Actual:   {actual_text[:80]}...")

        if exact:
            exact_matches += 1
        total_char_accuracy += char_acc

    print(f"\n{'='*60}")
    print(f"Results: {exact_matches}/{len(pairs)} exact matches "
          f"({exact_matches/len(pairs):.0%})")
    print(f"Average character accuracy: {total_char_accuracy/len(pairs):.1%}")

    sys.exit(0 if exact_matches == len(pairs) else 1)


if __name__ == '__main__':
    main()
