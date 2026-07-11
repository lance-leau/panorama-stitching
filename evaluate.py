import sys
import math
from pathlib import Path
import cv2
import numpy as np
from skimage.metrics import structural_similarity as ssim_fn

from srcPy.load_images import load_images
from srcPy.detect_points import detect_points
from srcPy.match_points import match_points
from srcPy.estimate_homography import estimate_homography


def evaluate_pair(img1, img2, label=""):
    kp1, d1 = detect_points(img1)
    kp2, d2 = detect_points(img2)
    matches  = match_points(d1, d2)
    H, inliers = estimate_homography(kp1, kp2, matches)

    pts_src = np.float32([kp2[m.queryIdx].pt for m, ok in zip(matches, inliers) if ok])
    pts_dst = np.float32([kp1[m.trainIdx].pt for m, ok in zip(matches, inliers) if ok])
    proj = cv2.perspectiveTransform(pts_src.reshape(-1, 1, 2), H).reshape(-1, 2)
    rmse = math.sqrt(np.mean(np.sum((proj - pts_dst) ** 2, axis=1)))

    h, w = img1.shape[:2]
    warped   = cv2.warpPerspective(img2, H, (w, h))
    overlap  = (cv2.cvtColor(img1,   cv2.COLOR_BGR2GRAY) > 0) & \
               (cv2.cvtColor(warped, cv2.COLOR_BGR2GRAY) > 0)
    ys, xs   = np.where(overlap)
    r0, r1, c0, c1 = ys.min(), ys.max(), xs.min(), xs.max()
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    p1 = clahe.apply(cv2.cvtColor(img1[r0:r1, c0:c1],   cv2.COLOR_BGR2GRAY))
    p2 = clahe.apply(cv2.cvtColor(warped[r0:r1, c0:c1], cv2.COLOR_BGR2GRAY))
    score, _ = ssim_fn(p1, p2, full=True)

    print(f"{label or 'paire'}")
    print(f"  Matches / inliers : {len(matches)} / {inliers.sum()} ({inliers.mean()*100:.0f}%)")
    print(f"  RMSE géom. : {rmse:.2f} px  {': excellent' if rmse<2 else '~ acceptable' if rmse<5 else ': mauvais'}")
    print(f"  SSIM recouvrement : {score:.3f}    {': excellent' if score>0.90 else '~ acceptable' if score>0.75 else ': mauvais'}")
    return rmse, score


if __name__ == "__main__":
    paths = [Path(p) for p in sys.argv[1:]]
    if len(paths) < 2:
        print("Usage: python evaluate.py img1.jpg img2.jpg [img3.jpg ...]")
        sys.exit(1)
    images = load_images(paths)
    for i in range(len(images) - 1):
        evaluate_pair(images[i], images[i+1], f"{paths[i].name} -> {paths[i+1].name}")
