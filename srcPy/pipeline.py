import numpy as np
from tqdm import tqdm

from .blend_images import blend_images, blend_multiple
from .detect_points import detect_points
from .estimate_homography import estimate_homography
from .load_images import load_images, save_image
from .match_points import match_points
from .warp_images import full_warp, warp_all_images


def align_pair(base, image):
    keypoints_base, descriptors_base = detect_points(base)
    keypoints_image, descriptors_image = detect_points(image)
    matches = match_points(descriptors_base, descriptors_image)
    homography, inliers = estimate_homography(keypoints_base, keypoints_image, matches)
    return homography, matches, inliers


def merge_pair(base, image):
    homography, matches, inliers = align_pair(base, image)
    warped_base, warped_image, base_mask, image_mask = full_warp(base, image, homography)
    panorama = blend_images(warped_base, warped_image, base_mask, image_mask)
    info = {
        "matches": len(matches),
        "inliers": int(inliers.sum()),
        "inlier_ratio": float(inliers.mean()),
    }
    return panorama, info


def _build_absolute_homographies(pairwise_H, ref):
    n = len(pairwise_H) + 1
    abs_H = [None] * n
    abs_H[ref] = np.eye(3, dtype=np.float64)
    for i in range(ref - 1, -1, -1):
        abs_H[i] = abs_H[i + 1] @ np.linalg.inv(pairwise_H[i])

    for i in range(ref + 1, n):
        abs_H[i] = abs_H[i - 1] @ pairwise_H[i - 1]

    return abs_H


def stitch_images(images):
    n = len(images)
    if n < 2:
        raise ValueError("il faut au moins deux images")

    if n == 2:
        panorama, info = merge_pair(images[0], images[1])
        return panorama, [info]
    
    pairwise_H = []
    infos = []
    for i in tqdm(range(n - 1), desc="alignement"):
        h, matches, inliers = align_pair(images[i], images[i + 1])
        pairwise_H.append(h)
        infos.append({
            "matches": len(matches),
            "inliers": int(inliers.sum()),
            "inlier_ratio": float(inliers.mean()),
        })

    ref = n // 2
    abs_H = _build_absolute_homographies(pairwise_H, ref)

    warped = warp_all_images(images, abs_H)
    panorama = blend_multiple(warped)
    return panorama, infos


def run_pipeline(paths, output_path, max_width=1600):
    images = load_images(paths, max_width=max_width)
    panorama, infos = stitch_images(images)
    save_image(output_path, panorama)
    return panorama, infos
