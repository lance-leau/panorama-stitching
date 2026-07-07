import cv2


def to_gray(image):
    return cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)


def create_detector():
    if hasattr(cv2, "SIFT_create"):
        return cv2.SIFT_create(nfeatures=3000)
    return cv2.ORB_create(nfeatures=3000)


def detect_points(image):
    gray = to_gray(image)
    detector = create_detector()
    keypoints, descriptors = detector.detectAndCompute(gray, None)
    if descriptors is None:
        return [], None
    return keypoints, descriptors
