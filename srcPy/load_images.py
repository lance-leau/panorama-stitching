from pathlib import Path

import cv2


def list_image_paths(folder):
    folder = Path(folder)
    extensions = ["*.jpg", "*.jpeg", "*.png", "*.bmp"]
    paths = []
    for extension in extensions:
        paths.extend(folder.glob(extension))
    return sorted(paths)


def read_image(path):
    image = cv2.imread(str(path), cv2.IMREAD_COLOR)
    if image is None:
        raise ValueError(f"image illisible: {path}")
    return image


def resize_if_large(image, max_width=1600):
    height, width = image.shape[:2]
    if width <= max_width:
        return image
    scale = max_width / width
    new_size = (max_width, int(height * scale))
    return cv2.resize(image, new_size, interpolation=cv2.INTER_AREA)


def load_images(paths, max_width=1600):
    images = []
    for path in paths:
        image = read_image(path)
        image = resize_if_large(image, max_width)
        images.append(image)
    return images


def save_image(path, image):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    ok = cv2.imwrite(str(path), image)
    if not ok:
        raise ValueError(f"impossible de sauvegarder: {path}")
