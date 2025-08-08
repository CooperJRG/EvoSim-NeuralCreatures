import argparse
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

from .environment import Grid


def load_grids(paths: List[Path]) -> List[Grid]:
    """Load grid CSV files into :class:`Grid` objects."""
    grids: List[Grid] = []
    for path in paths:
        grids.append(Grid.from_csv(path))
    return grids


def animate_grids(grids: List[Grid], interval: int) -> None:
    """Animate a sequence of grids endlessly."""
    images = [grid._to_image() for grid in grids]
    fig, ax = plt.subplots()
    im = ax.imshow(images[0], interpolation="none")
    ax.axis("off")

    def update(frame: int):
        im.set_array(images[frame])
        return [im]

    FuncAnimation(fig, update, frames=len(images), interval=interval, repeat=True)
    plt.show()


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Play back a series of grid CSVs endlessly as a screen saver"
    )
    parser.add_argument(
        "directory",
        type=Path,
        help="Directory containing grid CSV files for a single generation",
    )
    parser.add_argument(
        "--interval",
        type=int,
        default=200,
        help="Delay between frames in milliseconds",
    )
    args = parser.parse_args()

    csv_files = sorted(args.directory.glob("*.csv"))
    if not csv_files:
        raise SystemExit("No CSV files found in the specified directory")

    grids = load_grids(csv_files)
    animate_grids(grids, args.interval)


if __name__ == "__main__":
    main()
