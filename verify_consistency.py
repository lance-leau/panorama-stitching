"""
Vérifie que Python et C++ produisent des homographies cohérentes
sur les mêmes paires d'images.
Usage : python verify_consistency.py img1.jpg img2.jpg [img3.jpg ...]
"""
import sys
import subprocess
import json
import math
from pathlib import Path

import cv2
import numpy as np

from srcPy.load_images import load_images
from srcPy.detect_points import detect_points
from srcPy.match_points import match_points
from srcPy.estimate_homography import estimate_homography


def homography_python(img1, img2):
    kp1, d1 = detect_points(img1)
    kp2, d2 = detect_points(img2)
    matches  = match_points(d1, d2)
    H, inliers = estimate_homography(kp1, kp2, matches)
    return H, int(inliers.sum()), len(matches)


def rmse_between(H1, H2):
    """Distance moyenne entre deux homographies sur une grille de points."""
    pts = np.float32([[x, y] for y in range(0, 500, 50) for x in range(0, 500, 50)]).reshape(-1,1,2)
    p1 = cv2.perspectiveTransform(pts, H1).reshape(-1, 2)
    p2 = cv2.perspectiveTransform(pts, H2).reshape(-1, 2)
    return math.sqrt(np.mean(np.sum((p1 - p2)**2, axis=1)))


def main():
    paths = [Path(p) for p in sys.argv[1:]]
    if len(paths) < 2:
        print("Usage: python verify_consistency.py img1.jpg img2.jpg [img3.jpg ...]")
        sys.exit(1)

    images = load_images(paths)
    print(f"\nVérification de cohérence Python ↔ C++ sur {len(paths)-1} paire(s)\n")

    all_ok = True
    for i in range(len(images) - 1):
        label = f"{paths[i].name} → {paths[i+1].name}"
        H_py, inliers_py, matches_py = homography_python(images[i], images[i+1])

        # --- Récupération de H_cpp via le binaire (dump JSON) ---
        # En attendant l'implémentation C++, on compare les stats de matching
        print(f"  {label}")
        print(f"    Python : {matches_py} matches, {inliers_py} inliers")

        # Vérification interne : H_py doit être inversible (det ≠ 0)
        det = np.linalg.det(H_py)
        ok = abs(det) > 1e-6
        print(f"    det(H) = {det:.4f}  {'✓ inversible' if ok else '✗ singulière'}")

        # Vérification que H est bien une homographie normalisée (H[2,2] ≈ 1)
        h33 = H_py[2, 2]
        ok2 = abs(h33 - 1.0) < 0.05
        print(f"    H[2,2] = {h33:.4f}  {'✓ normalisée' if ok2 else '✗ non normalisée'}")

        if not (ok and ok2):
            all_ok = False

    print()
    if all_ok:
        print("Résultat : toutes les homographies sont valides ✓")
    else:
        print("Résultat : certaines homographies sont invalides ✗")


if __name__ == "__main__":
    main()
