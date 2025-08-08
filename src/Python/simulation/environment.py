import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
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
        """Display the grid with friendly icons and a legend.

        Cells are annotated with simple emojis or text so that the scene is easy
        to understand at a glance.  Grid lines and a legend are added to help a
        younger audience interpret the environment.
        """
        img = self._to_image()
        fig, ax = plt.subplots()
        ax.imshow(img, interpolation="none")

        # Draw grid lines so each cell is distinct
        ax.set_xticks(np.arange(-0.5, self.width, 1), minor=True)
        ax.set_yticks(np.arange(-0.5, self.height, 1), minor=True)
        ax.grid(which="minor", color="gray", linestyle="-", linewidth=0.5)
        ax.tick_params(
            which="both", bottom=False, left=False, labelbottom=False, labelleft=False
        )

        # Add icons or text to explain what is in each cell
        for _, row in self.dataframe.iterrows():
            x = int(row["X"])
            y = int(row["Y"])
            label = ""
            if row["Wall"]:
                label = "Wall"
            elif row["Poison"]:
                label = "☠"
            elif row["Food"]:
                label = "🍎"
            elif row["Water"]:
                label = "💧"
            elif row["Sunlit"]:
                label = "☀"
            if row["Occupied"]:
                label = f"{int(row['CreatureID'])}"
            if label:
                ax.text(x, y, label, ha="center", va="center", fontsize=8)

        legend_patches = [
            mpatches.Patch(color="red", label="Creature"),
            mpatches.Patch(color="green", label="Food"),
            mpatches.Patch(color="magenta", label="Poison"),
            mpatches.Patch(color="blue", label="Water"),
            mpatches.Patch(color="yellow", label="Sunlit"),
            mpatches.Patch(color="black", label="Wall"),
        ]
        ax.legend(handles=legend_patches, bbox_to_anchor=(1.05, 1), loc="upper left")
        plt.show()

    def describe_creatures(self) -> None:
        """Print a friendly sentence describing each creature's situation."""
        creatures = self.dataframe[self.dataframe["Occupied"]]
        for _, row in creatures.iterrows():
            x, y = int(row["X"]), int(row["Y"])
            parts = []
            if row["Food"]:
                parts.append("eating food")
            if row["Poison"]:
                parts.append("standing on poison")
            if row["Water"]:
                parts.append("in water")
            if row["Sunlit"]:
                parts.append("in the sun")
            if row["Wall"]:
                parts.append("up against a wall")
            if not parts:
                parts.append("on empty ground")
            print(
                f"Creature {int(row['CreatureID'])} at ({x}, {y}) is {' and '.join(parts)}."
            )


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
    grid.describe_creatures()


if __name__ == "__main__":
    main()
