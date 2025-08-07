"""Visualise a neural network exported by the C simulation.

This script reads ``neurons.csv`` and ``connections.csv`` files and produces a
NetworkX visualisation of the neural graph.  File paths can be supplied on the
command line; by default files in the same directory as the script are used.
"""

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
import pandas as pd


def build_graph(neuron_path: Path, connection_path: Path) -> nx.DiGraph:
    """Create a graph from neuron and connection CSV files."""
    neurons = pd.read_csv(neuron_path)
    connections = pd.read_csv(connection_path)

    graph = nx.DiGraph()
    for _, row in neurons.iterrows():
        graph.add_node(row["ID"], type=row["Type"], label=row["Label"])
    for _, row in connections.iterrows():
        graph.add_edge(
            row["SourceID"],
            row["TargetID"],
            weight=row["Weight"],
            activation_function=row["ActivationFunction"],
        )
    return graph


def draw_graph(graph: nx.DiGraph) -> None:
    """Render the graph using matplotlib."""
    pos = nx.spring_layout(graph, k=2, scale=2)
    node_colors = [
        "skyblue"
        if graph.nodes[node]["type"] == "SENSORY"
        else (
            "lightcoral" if graph.nodes[node]["type"] == "OUTPUT" else "lightgreen"
        )
        for node in graph.nodes
    ]
    labels = {node: graph.nodes[node]["label"] for node in graph.nodes()}
    nx.draw(graph, pos, labels=labels, node_color=node_colors, font_weight="bold",
            node_size=700, font_size=12)

    edge_labels = {k: round(v, 2) for k, v in nx.get_edge_attributes(graph, "weight").items()}
    nx.draw_networkx_edge_labels(graph, pos, edge_labels=edge_labels, font_size=14)

    for src, dst in graph.edges():
        path = [
            (x, y)
            for x, y in zip(
                np.linspace(pos[src][0], pos[dst][0], 100),
                np.linspace(pos[src][1], pos[dst][1], 100),
            )
        ]
        plt.annotate(
            "",
            xy=path[-1],
            xytext=path[0],
            arrowprops=dict(arrowstyle="->", color="black", lw=1.5),
            zorder=-1,
        )
    plt.show()


def main() -> None:
    parser = argparse.ArgumentParser(description="Visualise neural network from CSV files")
    default_dir = Path(__file__).resolve().parent
    parser.add_argument("--neurons", type=Path, default=default_dir / "neurons.csv",
                        help="Path to neurons.csv")
    parser.add_argument("--connections", type=Path, default=default_dir / "connections.csv",
                        help="Path to connections.csv")
    args = parser.parse_args()

    graph = build_graph(args.neurons, args.connections)
    draw_graph(graph)


if __name__ == "__main__":
    main()
