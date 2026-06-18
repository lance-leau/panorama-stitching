import cv2
import numpy as np


def image_corners(image):
    height, width = image.shape[:2]
    return np.float32([[0, 0], [width, 0], [width, height], [0, height]]).reshape(-1, 1, 2)


def transformed_corners(image, homography):
    return cv2.perspectiveTransform(image_corners(image), homography)


def canvas_from_corners(corners_a, corners_b):
    corners = np.vstack((corners_a, corners_b))
    x_min, y_min = np.floor(corners.min(axis=0).ravel()).astype(int)
    x_max, y_max = np.ceil(corners.max(axis=0).ravel()).astype(int)
    width = x_max - x_min
    height = y_max - y_min
    translation = np.array([[1, 0, -x_min], [0, 1, -y_min], [0, 0, 1]], dtype=np.float64)
    return width, height, translation


def full_warp(base, image, homography):
    base_corners = transformed_corners(base, np.eye(3))
    image_corners_warped = transformed_corners(image, homography)
    width, height, translation = canvas_from_corners(base_corners, image_corners_warped)
    warped_base = cv2.warpPerspective(base, translation, (width, height))
    warped_image = cv2.warpPerspective(image, translation @ homography, (width, height))
    base_mask = make_mask(base, translation, width, height)
    image_mask = make_mask(image, translation @ homography, width, height)
    return warped_base, warped_image, base_mask, image_mask


def make_mask(image, homography, width, height):
    mask = np.ones(image.shape[:2], dtype=np.uint8) * 255
    warped = cv2.warpPerspective(mask, homography, (width, height))
    return warped > 0
