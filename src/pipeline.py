from tqdm import tqdm

from .blend_images import blend_images
from .detect_points import detect_points
from .estimate_homography import estimate_homography
from .load_images import load_images, save_image
from .match_points import match_points
from .warp_images import full_warp


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


def stitch_images(images):
    if len(images) < 2:
        raise ValueError("il faut au moins deux images")
    panorama = images[0]
    infos = []
    for image in tqdm(images[1:], desc="assemblage"):
        panorama, info = merge_pair(panorama, image)
        infos.append(info)
    return panorama, infos


def run_pipeline(paths, output_path, max_width=1600):
    images = load_images(paths, max_width=max_width)
    panorama, infos = stitch_images(images)
    save_image(output_path, panorama)
    return panorama, infos
