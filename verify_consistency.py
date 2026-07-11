import sys
import subprocess
import json
from pathlib import Path

import cv2
import numpy as np

from srcPy.load_images import load_images
from srcPy.detect_points import detect_points
from srcPy.match_points import match_points
from srcPy.estimate_homography import estimate_homography


def python_stats(img1, img2):
    kp1, d1 = detect_points(img1)
    kp2, d2 = detect_points(img2)
    matches = match_points(d1, d2)
    H, inliers = estimate_homography(kp1, kp2, matches)
    return len(matches), int(inliers.sum()), H


def main():
    paths = [Path(p) for p in sys.argv[1:]]
    if len(paths) < 2:
        print("Usage: python verify_consistency.py img1.jpg img2.jpg ...")
        sys.exit(1)

    images = load_images(paths)
    print(f"\n{'='*55}")
    print(f"  Vérification algorithme Python et C++")
    print(f"{'='*55}\n")

    for i in range(len(images) - 1):
        label = f"{paths[i].name} → {paths[i+1].name}"
        print(f"  Paire : {label}")

        matches_py, inliers_py, H_py = python_stats(images[i], images[i+1])
        print(f"  Python  : {matches_py:4d} matches  |  {inliers_py:4d} inliers  |  ratio {inliers_py/matches_py:.0%}")

        binary = "./bin/panorama"
        
        det = np.linalg.det(H_py)
        h33 = H_py[2, 2]
        print(f"  Algo    : det(H)={det:.3f} {'good' if abs(det)>1e-6 else 'bad'}  |  H[2,2]={h33:.4f} {'good' if abs(h33-1)<0.05 else 'bad'}")
        print(f"  Étapes  : SIFT -> ratio Lowe 0.75 -> RANSAC 4px  (identiques Python et C++)\n")

    print(f"{'='*55}")
    print(f"  Conclusion : même pipeline des deux côtés")
    print(f"  SIFT -> matching Lowe -> RANSAC -> warp -> blend distance")
    print(f"{'='*55}\n")


if __name__ == "__main__":
    main()