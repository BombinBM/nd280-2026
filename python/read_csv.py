import os

import pandas as pd

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

# files = ["antimuon_data.csv", "proton_data.csv", "pion-_data.csv", "pion+_data.csv", "electron_data.csv", "muon_data.csv", "kaon-_data.csv", "kaon+_data.csv"]
files = ["antimuon_data.csv", "proton_data.csv"]
# Read CSV
df = pd.concat([pd.read_csv(file) for file in files if os.path.exists(file)], ignore_index=True)

# Expected columns: PDG, entered_hits,total_charge,average_charge,min_time,max_time

required_columns = ["PDG", "entered_hits", "total_charge", "average_charge", "min_time", "max_time"]

missing = [col for col in required_columns if col not in df.columns]

if missing:
    raise ValueError(f"CSV is missing required columns: {missing}. Expected columns: {required_columns}")

# Create 3D plot
fig = plt.figure(figsize=(8, 6))
ax = fig.add_subplot(111, projection="3d")

ax.scatter(df["entered_hits"], df["total_charge"], df["average_charge"], c=df["PDG"], cmap='plasma_r')

ax.set_xlabel("Entered Hits")
ax.set_ylabel("Total Charge")
ax.set_zlabel("Average Charge")
ax.set_title("3D Plot from CSV")

plt.tight_layout()
plt.show()