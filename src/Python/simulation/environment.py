import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


class Grid:
    """Representation of the simulation grid used by the C program.

    The grid CSV exported by :func:`output_grid_to_csv` contains one row per
    cell with the following columns::

        X,Y,Occupied,Food,Poison,Wall,Sunlit,Water,CreatureID

    This class loads that CSV and provides a simple visualisation of the
    current grid state.
    """

    def __init__(self, dataframe: pd.DataFrame):
        self.dataframe = dataframe
        self.width = int(dataframe["X"].max() + 1)
        self.height = int(dataframe["Y"].max() + 1)

    @classmethod
    def from_csv(cls, path: Path) -> "Grid":
        """Create a :class:`Grid` instance from a CSV file."""
        df = pd.read_csv(path)
        return cls(df)

    def _to_image(self) -> np.ndarray:
        """Convert the grid to an RGB image for plotting."""
        img = np.ones((self.height, self.width, 3), dtype=float)
        for _, row in self.dataframe.iterrows():
            x = int(row["X"])
            y = int(row["Y"])
            if row["Wall"]:
                colour = (0.0, 0.0, 0.0)
            elif row["Poison"]:
                colour = (1.0, 0.0, 1.0)
            elif row["Food"]:
                colour = (0.0, 1.0, 0.0)
            elif row["Water"]:
                colour = (0.0, 0.0, 1.0)
            elif row["Occupied"]:
                colour = (1.0, 0.0, 0.0)
            elif row["Sunlit"]:
                colour = (1.0, 1.0, 0.0)
            else:
                colour = (1.0, 1.0, 1.0)
            img[y, x] = colour
        return img

    def plot(self) -> None:
        """Display the grid using matplotlib."""
        img = self._to_image()
        plt.imshow(img, interpolation="none")
        plt.axis("off")
        plt.show()


def main() -> None:
    parser = argparse.ArgumentParser(description="Visualise grid state from CSV")
    parser.add_argument(
        "grid_csv",
        nargs="?",
        default=Path(__file__).with_name("grid.csv"),
        type=Path,
        help="Path to the grid CSV file",
    )
    args = parser.parse_args()

    grid = Grid.from_csv(args.grid_csv)
    grid.plot()


if __name__ == "__main__":
    main()
