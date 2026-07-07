import argparse
from pathlib import Path

from src.load_images import list_image_paths
from src.pipeline import run_pipeline


def next_output_path():
    output_dir = Path("outputs")
    output_dir.mkdir(exist_ok=True)
    existing = list(output_dir.glob("panorama*.jpg"))
    return output_dir / f"panorama{len(existing) + 1:02d}.jpg"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("inputs", nargs="+", help="images ou dossier contenant les images")
    parser.add_argument("--max-width", type=int, default=1600)
    return parser.parse_args()


def collect_paths(inputs):
    paths = []
    for item in inputs:
        path = Path(item)
        if path.is_dir():
            paths.extend(list_image_paths(path))
        else:
            paths.append(path)
    return sorted(paths)


def main():
    args = parse_args()
    paths = collect_paths(args.inputs)
    output = next_output_path()
    panorama, infos = run_pipeline(paths, output, args.max_width)
    print(f"panorama sauvegardé: {output}")
    print(f"taille: {panorama.shape[1]}x{panorama.shape[0]}")
    for index, info in enumerate(infos, start=1):
        print(f"fusion {index}: {info['matches']} matches, {info['inliers']} inliers")


if __name__ == "__main__":
    main()