import cv2
import numpy as np


def distance_weight(mask):
    mask_u8 = mask.astype(np.uint8)
    weight = cv2.distanceTransform(mask_u8, cv2.DIST_L2, 5)
    if weight.max() > 0:
        weight = weight / weight.max()
    return weight


def expand_weight(weight):
    return weight[:, :, None].astype(np.float32)


def blend_images(image_a, image_b, mask_a, mask_b):
    weight_a = distance_weight(mask_a)
    weight_b = distance_weight(mask_b)
    total = weight_a + weight_b
    total[total == 0] = 1
    weight_a = expand_weight(weight_a / total)
    weight_b = expand_weight(weight_b / total)
    blended = image_a.astype(np.float32) * weight_a + image_b.astype(np.float32) * weight_b
    return np.clip(blended, 0, 255).astype(np.uint8)
