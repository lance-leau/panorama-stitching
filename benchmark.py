"""
Comparaison de performances Python vs C++.
Usage : python benchmark.py img1.jpg img2.jpg [img3.jpg ...]
"""
import sys
import time
import subprocess
from pathlib import Path

import cv2

from srcPy.load_images import load_images
from srcPy.pipeline import stitch_images


def bench_python(paths):
    images = load_images(paths)
    t0 = time.perf_counter()
    stitch_images(images)
    return time.perf_counter() - t0


def bench_cpp(paths, binary="./bin/panorama"):
    if not Path(binary).exists():
        return None
    cmd = [binary] + [str(p) for p in paths]
    t0 = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True)
    elapsed = time.perf_counter() - t0
    if result.returncode != 0:
        print(f"  [ERREUR C++] {result.stderr.decode()}")
        return None
    return elapsed


def main():
    paths = [Path(p) for p in sys.argv[1:]]
    if len(paths) < 2:
        print("Usage: python benchmark.py img1.jpg img2.jpg [img3.jpg ...]")
        sys.exit(1)

    print(f"\nBenchmark sur {len(paths)} images\n")

    t_py  = bench_python(paths)
    t_cpp = bench_cpp(paths)

    print(f"  Python : {t_py:.2f}s")
    if t_cpp:
        print(f"  C++    : {t_cpp:.2f}s")
        print(f"  Speedup: x{t_py / t_cpp:.1f}")
    else:
        print(f"  C++    : binaire introuvable — compilez d'abord avec 'make' dans cpp/")


if __name__ == "__main__":
    main()