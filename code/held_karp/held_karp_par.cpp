/*  Parallel Held-Karp Algorithm for the Metric TSP Problem
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <float.h>
#include <omp.h>
#include "../parse/parser.h"

using namespace std;


// Global variables
int n;
vector<vector<float> > G;


/*  Return last row of Pascal's triangle
    This function takes on the order of 1e-6 seconds so no point
    trying to optimize it any further
*/
vector<int> pascals_triangle(int n) {
    vector<vector<int> > T(n, vector<int>(n, 0));
    T[0][0] = 1;
    for (int i = 1; i < n; i++) {
        T[i][0] = 1;
        for (int j = 1; j < i; j++) {
            T[i][j] = T[i - 1][j - 1] + T[i - 1][j];
        }
        T[i][i] = 1;
    }
    return T[n - 1];
}


int main(int argc, char *argv[]) {
    string file_name = "";
    int num_threads = omp_get_max_threads();
    // Check if thread count is passed in as a command line argument
    for (int i = 0; i < argc; i++) {
        string arg(argv[i]);
        if (arg == "-f" && i + 1 < argc) {
            file_name = argv[i + 1];
        } else if (arg == "-t" && i + 1 < argc) {
            num_threads = atoi(argv[i + 1]);
        }
    }
    omp_set_num_threads(num_threads);
    cout << "Running with " << num_threads << " threads" << endl;

    if (file_name == "") {
        cout << "Please specify a filename by adding -f [FILE_NAME]" << endl;
        return 0;
    }

    n = parse_matrix(file_name, G);

    // Allocate DP array
    float **C = (float**)malloc((1 << n) * sizeof(float*));
    for (int i = 0; i < (1 << n); i++) {
        C[i] = (float*)malloc(n * sizeof(float));
    }

    /*  Precompute last row of Pascal's triangle for values of n choose p
        in order to statically parallelize for loop in main computation */
    vector<int> T = pascals_triangle(n + 1);

    // Allocate array to store sets (removes sequential dependency on S in for loop)
    unsigned int **sets = (unsigned int **)malloc(n * sizeof(unsigned int*));
    for (int p = 2; p < n; p++) {
        sets[p] = (unsigned int *)malloc(T[p] * sizeof(unsigned int));
    }

    /*  Fill sets array, dynamic scheduling because number of sets for
        each size p can vary widely (Pascal's triangle) */
    #pragma omp parallel for schedule(dynamic)
    for (int p = 2; p < n; p++) {
        unsigned int S = (1 << p) - 1;
        int limit = 1 << n;
        int i = 0;
        while (S < limit) {
            sets[p][i] = S;
            i++;
            // Compute the next set using Gosper's Hack
            unsigned int c = S & -S;
            unsigned int r = S + c;
            S = (((r ^ S) >> 2) / c) | r;
        }
    }

    // First step of Held-Karp: compute base cases
    for (int k = 1; k < n; k++) {
        C[1 << k][k] = G[0][k];
    }

    /*  Main loop of Held-Karp: compute all subproblems via bottom-up DP
        Outer-most loop cannot be parallelized because larger subproblems 
        depend on smaller ones */
    for (int p = 2; p < n; p++) {
        /*  For all S a subset of {1, 2, ..., n - 1} such that |S| = p
            This is the loop to target for parallelism */
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < T[p]; i++) {
            unsigned int S = sets[p][i];
            if (!(S & 1)) {
                // For all k in S
                for (unsigned int k = 0; k < n; k++) {
                    if (S & (1 << k)) {
                        float min_cost = FLT_MAX;
                        // For all w in S, w != k
                        for (unsigned int w = 0; w < n; w++) {
                            if (w != k && S & (1 << w)) {
                                float cost = C[S & ~(1 << k)][w] + G[w][k];
                                if (cost < min_cost) {
                                    min_cost = cost;
                                }
                            }
                        }
                        C[S][k] = min_cost;
                    }
                }
            }
        }
        free(sets[p]);
    }
    free(sets);

    // Use computed subproblems to find the optimal cost
    float opt_cost = FLT_MAX;
    unsigned int S_tour = ((1 << n) - 1) & ~1;
    for (int k = 1; k < n; k++) {
        float tour_cost = C[S_tour][k] + G[k][0];
        if (tour_cost < opt_cost) {
            opt_cost = tour_cost;
        }
    }

    // Output optimal cost
    cout << "Tour cost = " << opt_cost << endl;

    // Free memory
    for (int i = 0; i < (1 << n); i++) {
        free(C[i]);
    }
    free(C);

    return 0;
}
