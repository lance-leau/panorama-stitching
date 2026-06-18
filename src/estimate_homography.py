import cv2
import numpy as np


def points_from_matches(keypoints_a, keypoints_b, matches):
    points_b = np.float32([keypoints_b[match.queryIdx].pt for match in matches])
    points_a = np.float32([keypoints_a[match.trainIdx].pt for match in matches])
    return points_b, points_a


def estimate_homography(keypoints_a, keypoints_b, matches, threshold=4.0):
    if len(matches) < 4:
        raise ValueError("pas assez de correspondances")
    source, destination = points_from_matches(keypoints_a, keypoints_b, matches)
    homography, mask = cv2.findHomography(source, destination, cv2.RANSAC, threshold)
    if homography is None:
        raise ValueError("homographie impossible à estimer")
    return homography, mask.ravel().astype(bool)
