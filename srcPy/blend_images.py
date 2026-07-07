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


def compensate_exposure(warped_images_masks):
    n = len(warped_images_masks)
    if n < 2:
        return warped_images_masks

    gains = [1.0] * n
    ref_img, ref_mask = warped_images_masks[0]

    for i in range(1, n):
        img_i, mask_i = warped_images_masks[i]
        overlap = ref_mask & mask_i
        if overlap.sum() < 100:
            continue
        mean_ref = ref_img[overlap].mean()
        mean_i = img_i[overlap].mean()
        if mean_i > 1e-3:
            gains[i] = float(mean_ref / mean_i)

    result = []
    for i, (img, mask) in enumerate(warped_images_masks):
        g = gains[i]
        if abs(g - 1.0) < 0.01:
            result.append((img, mask))
        else:
            corrected = np.clip(img.astype(np.float32) * g, 0, 255).astype(np.uint8)
            result.append((corrected, mask))
    return result


def blend_multiple(warped_images_masks):
    warped_images_masks = compensate_exposure(warped_images_masks)

    weights = []
    for img, mask in warped_images_masks:
        w = cv2.distanceTransform(mask.astype(np.uint8), cv2.DIST_L2, 5).astype(np.float32)
        weights.append(w)

    total = sum(weights)
    total[total == 0] = 1.0

    h, w = warped_images_masks[0][0].shape[:2]
    result = np.zeros((h, w, 3), dtype=np.float32)
    for (img, _), weight in zip(warped_images_masks, weights):
        result += img.astype(np.float32) * (weight / total)[:, :, None]

    return np.clip(result, 0, 255).astype(np.uint8)
