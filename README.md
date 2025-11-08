This project implements the K-Means clustering algorithm entirely in C.
It reads a dataset from a CSV file, performs clustering based on the number of clusters specified by the user, and outputs the results along with a Python script that visualizes the clusters in 2D using PCA (Principal Component Analysis).

*2 files:*
main.c – Contains the main implementation of the K-Means algorithm, file handling, and automatic generation of a Python script for visualization.
FileHandling.c / FileHandling.h – Contain helper functions for reading and processing the dataset (e.g., counting rows and columns, reading numeric values, etc.).

*How It Works:*
1. The program asks the user to input the dataset filename (e.g., data.csv).
2. It detects the number of rows and columns automatically.
3. The user specifies how many clusters (k) to create.
4. The K-Means algorithm runs until convergence or until the maximum number of iterations (1000) is reached.
5. The program:
      i) Prints execution time and total within-cluster error.
      ii) Saves the clustered data into files C0.txt, C1.txt, …, and the centroids into Centroids.txt.
      iii) Automatically generates and runs a Python script (plot_clusters.py) that visualizes the results using PCA.

*Requirements*
C compiler (e.g., GCC)
Python 3 installed with the following packages:  pip install matplotlib numpy scikit-learn

*How to Run*
1. Compile the code: gcc main.c FileHandling.c -o kmeans -lm
2. Run the program: ./kmeans
3. Enter your dataset filename (e.g., dataset.csv) and the number of clusters.
4. After execution, the results will be written to: C0.txt, C1.txt, … (clustered data), Centroids.txt (final centroids), plot_clusters.py (Python visualization script)
5. To visualize the results: python plot_clusters.py

*Example Output*

Give the DataSet file: data.csv
Elements: 200
Dimensions: 4
Choose the amount of Clusters: 3

Time of Algorithm Execution: 0.003241
Iterations: 12

Total Within-Cluster Error: 47.391820
Cluster 0: 66 points written
Cluster 1: 67 points written
Cluster 2: 67 points written

**Notes**
**The dataset must contain only numeric values separated by commas.
**The Python visualization uses PCA to project high-dimensional data to 2D for plotting.
