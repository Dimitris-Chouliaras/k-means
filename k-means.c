#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{

    int n = 0;           // n Elements  // For all loops always i < n !!!WARNING SOS!!!              // return 0 --> programm ended
    int k;               // k clusters                                                               // return -1 --> problem with the given dataset file
    int i, j, d;         // i counter for n, j counter for clusters, d counter for dimensions        // return -2 --> problem with the initial file
    char filename[100];  // The Variable for loading Custom Dataset                                  // return -3 --> problem with the error file
    int dim = 0;         // Dimensions of Elements                                                   // return -4 --> problem with the error file 2.0
    char c;              // Temporary character for file analysis                                    // return -5 --> problem with the final file
    int a = 1;           // Use a = 1 to generate random Centroids                                   // return -6 --> problem with the result file
		int iteration = 0;   // Amount of repetitions
		int flag_end = 0;    // Flag for ending k-means
		int flag = -1;       // Flag for filtering
    clock_t start, end;  // Count the time
    int q;               // Used in switch

// srand(time(NULL));

/* -------------------------------------------------------------------------- */
    // Reading Dataset, counting n elements, assigning to X Array

   FILE* Dataset;

      printf("\n Give the DataSet file:");
      scanf("%s", filename);

    Dataset = fopen(filename, "r");
    if (!Dataset)
    {
        printf("\n There is something wrong with the Dataset file! \n\n");
        return -1;
    }

/* -------------------------------------------------------------------------- */
    // Finding the dimensions of Elements

    printf("\n Dataset is dimension seperated with: ");
    printf("\n 1. Comma (,) ");
    printf("\n 2. Space ( ) ");
    printf("\n 3. Tab ");
    printf("\n Choose your answer : ");
    scanf("%d",&q);

    switch (q)
    {
    case 1:
             do
             {
               fscanf(Dataset, "%c", &c);
               if (c == ',')
               dim++;
             } while (c != '\n');
             dim++;
             rewind(Dataset);  // File reset
    break;
    case 2:
             do
             {
               fscanf(Dataset, "%c", &c);
               if (c == ' ')
               dim++;
             } while (c != '\n');
             dim++;
             rewind(Dataset);  // File reset
    break;
    case 3:
            do
            {
              fscanf(Dataset, "%c", &c);
              if (c == '\t')
              dim++;
            }while (c != '\n');
            dim++;
            rewind(Dataset);  // File reset
    break;
    }

    // Finding the number of Elements

    do
    {
        fscanf(Dataset, "%c", &c);
        if (c == '\n')
            n++;
    } while (!feof(Dataset));

    rewind(Dataset); // File reset
    n--;

    printf("\n Elements:%d \n", n);
    printf("\n Dimensions:%d \n", dim);

/* -------------------------------------------------------------------------- */
    printf("\n Choose the amount of Clusters:");
    scanf("%d", &k);
/* -------------------------------------------------------------------------- */
    // All the necessary memory allocation

    double **X;  // Array of Elements
    X = calloc(n, sizeof(double));
    for (d = 0; d < n; d++)
        X[d] = calloc(dim, sizeof(double));

    double **V;  // Array of Centroids
    V = calloc(k, sizeof(double));
    for (d = 0; d < k; d++)
        V[d] = calloc(dim, sizeof(double));

    double **C;  // Array of Clusters                                            //C[n][dim+1] --> C[n][0] always stands for j
    C = calloc(k * (n), sizeof(double));
    for (d = 0; d <= k*(n); d++)
        C[d] = calloc((dim+1), sizeof(double));

    double **FlagCentroids;  // Array of flag Centroids
    FlagCentroids = calloc(k, sizeof(double));
    for (d = 0; d < k; d++)
        FlagCentroids[d] = calloc(dim, sizeof(double));

		int *counter;  // Counter of elements for each cluster
    counter = calloc(k, sizeof(int));

		double **totalCluster;  // Sum of every element for each cluster
    totalCluster= calloc(k,sizeof(double));
		for(d = 0; d < k; d++)
		   totalCluster[d] = calloc(dim,sizeof(double));

		double **distance;  // Array of Distance
		distance = calloc(n,sizeof(double));
			 for(j = 0; j < n; j++)
			 distance[j] = calloc(k,sizeof(double));

    double *min;  // Array of minimum distance
    min = calloc(n,sizeof(double));

    int *location;  // Array of Locations
    location = calloc(n,sizeof(int));

    FILE** Cluster = calloc(k+1,sizeof(FILE*));

    FILE** initialFile = calloc(k+1,sizeof(FILE*));

    FILE** finalFile = calloc(k+1,sizeof(FILE*));

    FILE* Centroids;

/* -------------------------------------------------------------------------- */
    // Passing elements to Array X[n][dim]

   while(!feof(Dataset))
    for (i = 0;  i < n; i++)
    {
        for (d = 0; d < dim; d++)
        {
            fscanf(Dataset, "%lf,", &X[i][d]);
        }
    }

    fclose(Dataset);

/* -------------------------------------------------------------------------- */
                        /* THIS IS OPTIONAL */
//Converting all values to Unsigned and Writing them into a file

// Dataset = fopen("ConvertedDataset.txt","w");
// for(i = 0; i < n; i++)
//    for(d = 0; d < dim; d++)
//    X[i][d] = fabs(X[i][d]);
//
//    for(i = 0; i < n; i++){
//          for(d = 0; d < dim; d++){
//              fprintf(Dataset, "%lf",X[i][d]);
//              if(d < dim - 1)
//              {
//                fprintf(Dataset,", ");
//              }
//            }
//            fprintf(Dataset, "\n" );
//            }
//
// fclose(Dataset);

/* -------------------------------------------------------------------------- */
    // Generating Initial Random Centroids

    for (j = 0; j < k; j++)
    {
      i = ((rand() % ((n) - a + 1)) + a);
        for (d = 0; d < dim; d++)
            V[j][d] = X[i][d];
    }

/* -------------------------------------------------------------------------- */
    // Assigning the initial random centroids to FlagCentroids

    for (j = 0; j < k; j++)
        for (d = 0; d < dim; d++)
            FlagCentroids[j][d] = V[j][d];

/* -------------------------------------------------------------------------- */
                     /* HERE STARTS K-MEANS */
start = clock();  // Starts timing the Algorithm

    do {
      //   Setting Sum and Counter of each cluster to 0

          for(j = 0; j < k; j++)
             for(d = 0; d < dim; d++)
               {
                 totalCluster[j][d] = 0;
                 counter[j] = 0;
               }

/* -------------------------------------------------------------------------- */
  // Calculating Distances of each element for each cluster and each dimension

          for(i = 0; i < n; i++)
             for(j = 0; j < k; j++)
               for(d = 0; d < dim; d++)
                 distance[i][j] += ((X[i][d] - V[j][d])*(X[i][d] - V[j][d]));

/* -------------------------------------------------------------------------- */
      // Comparing Distances and getting the proper Cluster for each Element

      for(i = 0; i < n; i++)
      {
          min[i]=distance[i][0];
          for(j = 0; j < k; j++)
          {
              if(distance[i][j]<=min[i])
              {
                   min[i]=distance[i][j];
                         location[i]=j;
              }
          }
      }

/* -------------------------------------------------------------------------- */
      // Getting Elements and Clusters all together

      for(i = 0; i < n; i++)
      C[i][0] = location[i];

      for(i = 0; i < n; i++)
         for(d = 1; d <= dim; d++)
       C[i][d] = X[i][d-1];

/* -------------------------------------------------------------------------- */
      // Calculating total Sum of each dim for each cluster and counter

       for(i = 0; i < n; i++)
          for(j = 0; j < k; j++)
             for(d = 0; d < dim; d++)
              if(C[i][0] == j)
              {
              totalCluster[j][d] += C[i][d+1];
              counter[j]++;
              }

/* -------------------------------------------------------------------------- */
    // Calculating New Centroid

              for(j = 0; j < k; j++)
                  for(d = 0; d < dim; d++)
                     V[j][d] = (totalCluster[j][d])/(counter[j]/dim);

/* -------------------------------------------------------------------------- */
			// Here goes the comparison last FlagCentroid with new centroids

                for(j = 0; j < k; j++)
                {
                    for(d = 0; d < dim; d++)
                    if(FlagCentroids[j][d] != V[j][d])
                    {
                        flag_end = 0;
                        break;
                    }else
                    {
                        flag_end = -1;
                        break;
                    }
                 }

/* -------------------------------------------------------------------------- */
       // Passing New centroids to flag;

                  for(j = 0; j < k; j++)
                    for(d = 0; d < dim; d++)
                    FlagCentroids[j][d] = V[j][d];

                    iteration++;

		} while(flag_end != -1);
end = clock();

double total_time = ((double) (end - start)) / CLOCKS_PER_SEC;

printf("\n\n Amount of Iterations: %d\n\n",iteration+1);

/* -------------------------------------------------------------------------- */
      // Writing to a file for Each cluster
start = clock();

   for(j = 0 ; j < k ; j++)
   {
      char *fileName;
      fileName = calloc(n,sizeof(char));
      snprintf(fileName,n*sizeof(char),"Cluster_%d.txt",j);
      Cluster[j] = fopen(fileName,"w");
      free(fileName);
   }


for(i = 0; i < n; i++){
   for( j = 0; j < k; j++){
      for(d = 1; d <= dim; d++){
          if(C[i][0] == j){
          fprintf(Cluster[j], "%lf",C[i][d]);
          if(d <= dim - 1)
          {
            fprintf(Cluster[j],", ");
          }
        }
        }
 fprintf(Cluster[j], "\n" );
 }
}

for (j = 0; j < k; j++)
fclose(Cluster[j]);

/* -------------------------------------------------------------------------- */
     //Creating and writing to a file the Final Centroids

Centroids = fopen("FinalCentroids.txt","w");

for(j = 0; j < k; j++){
    if(j != 0)
    fprintf(Centroids, "\n");
   for(d = 0; d < dim; d++){
   fprintf(Centroids, "%lf",FlagCentroids[j][d]);
   if(d < dim -1){
     fprintf(Centroids,", ");
   }
 }
}
   fclose(Centroids);

/* -------------------------------------------------------------------------- */
       // Deleting Empty rows and Blank rows

int MAX = n*k*dim;
char str[MAX];

for(j = 0 ; j < k ; j++)
{
  char *fileName;
 fileName = calloc(n,sizeof(char));
  snprintf(fileName,n*sizeof(char),"Cluster_%d.txt",j);
   initialFile[j] = fopen(fileName,"r");
   if (!initialFile[j])
   {
       printf("\n There is something wrong with the Initial file! \n\n");
       return -2;
   }
   free(fileName);
}

for(j = 0 ; j < k ; j++)
{
  char *fileName;
 fileName = calloc(n,sizeof(char));
  snprintf(fileName,n*sizeof(char),"c%d.txt",j);
   finalFile[j] = fopen(fileName,"w");
   free(fileName);
}

for(j = 0; j < k; j++)
{
   while(!feof(initialFile[j]))
   {
     fgets(str,MAX,initialFile[j]);
     if(strncmp(str,"\n",1) == 0)
     {
       continue;
     }
     fputs(str,finalFile[j]);
     strncpy(str, "\0",1);
   }
}

for(j = 0; j < k; j++)
fclose(initialFile[j]);

for(j = 0; j < k; j++)
fclose(finalFile[j]);

end = clock();

double total_time2 = ((double) (end - start)) / CLOCKS_PER_SEC;


/* -------------------------------------------------------------------------- */
       //Deleting Useless Files

for(j = 0 ; j < k ; j++)
{
  char *fileName;
 fileName = calloc(n,sizeof(char));
  snprintf(fileName,n*sizeof(char),"Cluster_%d.txt",j);
   remove(fileName);
   free(fileName);
}

for(j = 60 ; j >= k ; j--)
{
  char *fileName;
  fileName = calloc(n,sizeof(char));
  snprintf(fileName,n*sizeof(char),"c%d.txt",j);
   remove(fileName);
   free(fileName);
}


printf("\n Time of Algorithm Execution: %lf \n\n",total_time);
printf("\n Time of Writing Clusters to Files: %lf \n",total_time2);

/* -------------------------------------------------------------------------- */
                       /* Invoking Gnuplot */
// FILE *Gnuplot = fopen("autoPlot.gp","w");
// fprintf(Gnuplot, "plot ");
//
// for(j = 0 ; j < k ; j++)
// {
//   char *fileName;
//   fileName = calloc(n,sizeof(char));
//   snprintf(fileName,n*sizeof(char),"c%d.txt",j);
//   fprintf(Gnuplot, " \"%s\", ",fileName);
//   free(fileName);
// }
//
// fprintf(Gnuplot, "\"FinalCentroids.txt\"");
// fclose(Gnuplot);
// system("gnuplot -p 'autoPlot.gp'");

/* -------------------------------------------------------------------------- */
    // FREE EVERYTHING

    free(X);
    free(V);
    free(C);
    free(FlagCentroids);
		free(counter);
		free(totalCluster);
		free(distance);
    free(min);
    free(location);
    free(Cluster);
    free(initialFile);
    free(finalFile);

    return 0;
}
