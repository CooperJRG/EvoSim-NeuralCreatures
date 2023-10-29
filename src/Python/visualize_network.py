import pandas as pd
import matplotlib.pyplot as plt
import networkx as nx
import numpy as np

# Read CSV files
neurons = pd.read_csv('src/Python/neurons.csv')
connections = pd.read_csv('src/Python/connections.csv')

# Initialize graph
G = nx.DiGraph()

# Add nodes with attributes
for index, row in neurons.iterrows():
    G.add_node(row['ID'], type=row['Type'], label=row['Label'])

# Add edges with attributes
for index, row in connections.iterrows():
    G.add_edge(row['SourceID'], row['TargetID'], weight=row['Weight'], activation_function=row['ActivationFunction'])

# Generate positions for the nodes
pos = nx.spring_layout(G, k=2, scale=2)


# Draw the graph with customization
node_colors = ["skyblue" if G.nodes[node]['type'] == "SENSORY" else ("lightcoral" if G.nodes[node]['type'] == "OUTPUT" else "lightgreen") for node in G.nodes]

labels = {node: G.nodes[node]['label'] for node in G.nodes()}
nx.draw(G, pos, labels=labels, node_color=node_colors, font_weight='bold', node_size=700, font_size=12)


# Add edge labels
edge_labels = {k: round(v, 2) for k, v in nx.get_edge_attributes(G, 'weight').items()}
nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=14)

# Add curved arrows
for i, (src, dst) in enumerate(G.edges()):
    path = [(x, y) for x, y in zip(
        np.linspace(pos[src][0], pos[dst][0], 100),
        np.linspace(pos[src][1], pos[dst][1], 100)
    )]
    plt.annotate('', xy=path[-1], xytext=path[0],
                 arrowprops=dict(arrowstyle='->', color='black', lw=1.5),
                 zorder=-1)

plt.show()
