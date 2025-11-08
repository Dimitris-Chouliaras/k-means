#include <stdio.h>
#include <stdlib.h>
#include "FileHandling.h"
#include <math.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int n = 0, k, i, j, d;
    char filename[100];
    int dim = 0, iteration = 0, flag_end = 0;
    clock_t start, end;

    FILE *Dataset;

    printf("\nGive the DataSet file: ");
    scanf("%s", filename);

    Dataset = fopen(filename, "r");
    if (!Dataset)
    {
        printf("\nThere is something wrong with the Dataset file!\n\n");
        return -1;
    }

    dim = getColumns(Dataset);
    rewind(Dataset);

    n = getRows(Dataset);
    rewind(Dataset);

    printf("\nElements: %d\n", n);
    printf("Dimensions: %d\n", dim);

    printf("\nChoose the amount of Clusters: ");
    scanf("%d", &k);

    /* Memory allocation */
    double **X = calloc(n, sizeof(*X));
    for (i = 0; i < n; i++)
        X[i] = calloc(dim, sizeof(**X));

    double **V = calloc(k, sizeof(*V));
    for (j = 0; j < k; j++)
        V[j] = calloc(dim, sizeof(**V));

    double **FlagCentroids = calloc(k, sizeof(*FlagCentroids));
    for (j = 0; j < k; j++)
        FlagCentroids[j] = calloc(dim, sizeof(**FlagCentroids));

    int *counter = calloc(k, sizeof(*counter));

    double **totalCluster = calloc(k, sizeof(*totalCluster));
    for (j = 0; j < k; j++)
        totalCluster[j] = calloc(dim, sizeof(**totalCluster));

    double **distance = calloc(n, sizeof(*distance));
    for (i = 0; i < n; i++)
        distance[i] = calloc(k, sizeof(**distance));

    double *min = calloc(n, sizeof(*min));
    int *location = calloc(n, sizeof(*location));
    int *Cluster = calloc(n, sizeof(*Cluster));

    X = getData(Dataset, n, dim, X);
    fclose(Dataset);

    srand((unsigned)time(NULL));
    start = clock();

    /* Initialize random centroids */
    for (j = 0; j < k; j++)
    {
        i = rand() % n;
        for (d = 0; d < dim; d++)
            V[j][d] = X[i][d];
    }

    for (j = 0; j < k; j++)
        for (d = 0; d < dim; d++)
            FlagCentroids[j][d] = V[j][d];

    /* Main k-means loop */
    do
    {
        for (j = 0; j < k; j++)
        {
            counter[j] = 0;
            for (d = 0; d < dim; d++)
                totalCluster[j][d] = 0.0;
        }

        for (i = 0; i < n; i++)
            for (j = 0; j < k; j++)
                distance[i][j] = 0.0;

        /* Calculate distances */
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < k; j++)
            {
                double sum = 0.0;
                for (d = 0; d < dim; d++)
                    sum += (X[i][d] - V[j][d]) * (X[i][d] - V[j][d]);
                distance[i][j] = sqrt(sum);
            }
        }

        /* Assign clusters */
        for (i = 0; i < n; i++)
        {
            min[i] = distance[i][0];
            location[i] = 0;
            for (j = 1; j < k; j++)
            {
                if (distance[i][j] < min[i])
                {
                    min[i] = distance[i][j];
                    location[i] = j;
                }
            }
            Cluster[i] = location[i];
        }

        /* Sum and count per cluster */
        for (i = 0; i < n; i++)
        {
            int c = Cluster[i];
            counter[c]++;
            for (d = 0; d < dim; d++)
                totalCluster[c][d] += X[i][d];
        }

        /* Calculate new centroids */
        for (j = 0; j < k; j++)
        {
            if (counter[j] == 0)
            {
                int rand_index = rand() % n;
                for (d = 0; d < dim; d++)
                    V[j][d] = X[rand_index][d];
            }
            else
            {
                for (d = 0; d < dim; d++)
                    V[j][d] = totalCluster[j][d] / counter[j];
            }
        }

        /* Compare old & new centroids */
        flag_end = 1;
        for (j = 0; j < k && flag_end; j++)
        {
            for (d = 0; d < dim; d++)
            {
                if (fabs(FlagCentroids[j][d] - V[j][d]) > 1e-12)
                {
                    flag_end = 0;
                    break;
                }
            }
        }

        /* Update FlagCentroids */
        for (j = 0; j < k; j++)
            for (d = 0; d < dim; d++)
                FlagCentroids[j][d] = V[j][d];

        iteration++;
        if (iteration > 1000)
            break;

    } while (!flag_end);

    end = clock();
    double total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTime of Algorithm Execution: %lf\n", total_time);
    printf("Iterations: %d\n", iteration);

    /* Print final centroids */
    printf("\nFinal Centroids:\n");
    for (j = 0; j < k; j++)
    {
        printf("Centroid %d: ", j);
        for (d = 0; d < dim; d++)
            printf("%lf ", V[j][d]);
        printf("\n");
    }

    /* Calculate total within-cluster error */
    double total_error = 0.0;
    for (i = 0; i < n; i++)
    {
        int cl = Cluster[i];
        double dist = 0.0;
        for (d = 0; d < dim; d++)
            dist += (X[i][d] - V[cl][d]) * (X[i][d] - V[cl][d]);
        total_error += dist;
    }
    printf("\nTotal Within-Cluster Error: %lf\n", total_error);

    /* Write clusters to files */
    FILE **ClusterFile = calloc(k, sizeof(FILE *));
    for (j = 0; j < k; j++)
    {
        char fileName[32];
        snprintf(fileName, sizeof(fileName), "C%d.txt", j);
        ClusterFile[j] = fopen(fileName, "w");
        if (!ClusterFile[j])
            continue;

        int count = 0;
        for (i = 0; i < n; i++)
        {
            if (Cluster[i] == j)
            {
                for (d = 0; d < dim; d++)
                    fprintf(ClusterFile[j], "%lf ", X[i][d]);
                fprintf(ClusterFile[j], "\n");
                count++;
            }
        }
        printf("Cluster %d: %d points written\n", j, count);
        fclose(ClusterFile[j]);
    }
    free(ClusterFile);

    /* Write centroids to Centroids.txt */
    FILE *centFile = fopen("Centroids.txt", "w");
    for (j = 0; j < k; j++)
    {
        for (d = 0; d < dim; d++)
            fprintf(centFile, "%lf ", V[j][d]);
        fprintf(centFile, "\n");
    }
    fclose(centFile);

    /* Generate Python script for PCA 2D plotting */
    FILE *pyScript = fopen("plot_clusters.py", "w");
    if (pyScript)
    {
        fprintf(pyScript,
            "import matplotlib.pyplot as plt\n"
            "import numpy as np\n"
            "from sklearn.decomposition import PCA\n\n"
            "k = %d\n"
            "colors = ['red','blue','green','orange','purple','brown','cyan','magenta']\n\n"
            "clusters_data = []\n"
            "for i in range(k):\n"
            "    data = np.loadtxt(f'C{i}.txt')\n"
            "    if len(data.shape) == 1:\n"
            "        data = data.reshape(1, -1)\n"
            "    clusters_data.append(data)\n\n"
            "centroids = np.loadtxt('Centroids.txt')\n"
            "if len(centroids.shape) == 1:\n"
            "    centroids = centroids.reshape(1, -1)\n\n"
            "all_data = np.vstack(clusters_data)\n"
            "pca = PCA(n_components=2)\n"
            "pca.fit(all_data)\n"
            "all_data_2d = pca.transform(all_data)\n"
            "centroids_2d = pca.transform(centroids)\n\n"
            "start = 0\n"
            "clusters_2d = []\n"
            "for data in clusters_data:\n"
            "    end = start + data.shape[0]\n"
            "    clusters_2d.append(all_data_2d[start:end])\n"
            "    start = end\n\n"
            "plt.figure(figsize=(8,6))\n"
            "for i, data in enumerate(clusters_2d):\n"
            "    plt.scatter(data[:,0], data[:,1], c=colors[i], label=f'Cluster {i}', alpha=0.5)\n"
            "plt.scatter(centroids_2d[:,0], centroids_2d[:,1], c='black', marker='X', s=200, label='Centroids')\n"
            "plt.xlabel('PCA Component 1')\n"
            "plt.ylabel('PCA Component 2')\n"
            "plt.title('K-means Clusters (PCA 2D)')\n"
            "plt.legend()\n"
            "plt.grid(True)\n"
            "plt.show()\n", k);

        fclose(pyScript);

        /* Run the Python script */
        system("python plot_clusters.py");
    }

    /* Free memory */
    for (i = 0; i < n; i++) free(X[i]);
    free(X);
    for (j = 0; j < k; j++) free(V[j]);
    free(V);
    for (j = 0; j < k; j++) free(FlagCentroids[j]);
    free(FlagCentroids);
    for (j = 0; j < k; j++) free(totalCluster[j]);
    free(totalCluster);
    for (i = 0; i < n; i++) free(distance[i]);
    free(distance);
    free(counter);
    free(min);
    free(location);
    free(Cluster);

    return 0;
}

