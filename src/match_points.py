import cv2
import numpy as np


def matcher_for(descriptors):
    if descriptors.dtype == np.uint8:
        return cv2.BFMatcher(cv2.NORM_HAMMING)
    return cv2.BFMatcher(cv2.NORM_L2)


def keep_good_matches(raw_matches, ratio):
    good = []
    for pair in raw_matches:
        if len(pair) < 2:
            continue
        first, second = pair
        if first.distance < ratio * second.distance:
            good.append(first)
    return good


def sort_matches(matches):
    return sorted(matches, key=lambda match: match.distance)


def match_points(descriptors_a, descriptors_b, ratio=0.75):
    if descriptors_a is None or descriptors_b is None:
        return []
    matcher = matcher_for(descriptors_a)
    raw_matches = matcher.knnMatch(descriptors_b, descriptors_a, k=2)
    good_matches = keep_good_matches(raw_matches, ratio)
    return sort_matches(good_matches)
