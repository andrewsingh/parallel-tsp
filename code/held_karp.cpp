/*  Held-Karp Algorithm for the Metric TSP Problem 
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <vector>
#include <limits.h>
using namespace std;



// Return last row of Pascal's triangle
int *pascals_triangle(int n) {
    int **T = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        T[i] = (int*)malloc(n * sizeof(int));
    }
    T[0][0] = 1;
    for (int i = 1; i < n; i++) {
        T[i][0] = 1;
        // cout << T[i][0] << " ";
        for (int j = 1; j < i; j++) {
            T[i][j] = T[i - 1][j - 1] + T[i - 1][j];
            // cout << T[i][j] << " ";
        }
        T[i][i] = 1;
        // cout << T[i][i] << " ";
        // cout << endl;
    }
    int *T_last = T[n - 1];
    for (int i = 0; i < n - 1; i++) {
        free(T[i]);
    }
    free(T);
    return T_last;
}




int main() {
    // n = number of nodes
    int n;
    cin >> n;

    // Allocate weights matrix
    int **G = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        G[i] = (int*)malloc(n * sizeof(int));
    }

    // Read in weights matrix
    int dist;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> dist;
            G[i][j] = dist;
        }
    }

    // Allocate DP array
    int **C = (int**)malloc((1 << n) * sizeof(int*));
    for (int i = 0; i < (1 << n); i++) {
        C[i] = (int*)malloc(n * sizeof(int));
    }

    // Precompute last row of Pascal's triangle for values of n choose p
    // in order to statically parallelize for loop in main computation
    int *T = pascals_triangle(n + 1);

    // First step of Held-Karp: compute base cases
    for (int k = 1; k < n; k++) {
        C[1 << k][k] = G[0][k];
    }

    // Main loop of Held-Karp: compute all subproblems via bottom-up DP
    // Outer loop cannot be parallelized because larger subproblems depend on smaller ones
    for (int p = 2; p < n; p++) {
        unsigned int S = (1 << p) - 1;
        int limit = 1 << n;
        // For all S a subset of {1, 2, ..., n - 1} such that |S| = p
        // This loop can be parallelized
        for (int i = 0; i < T[p]; i++) {
            if (!(S & 1)) {
                // For all k in S
                for (unsigned int k = 0; k < n; k++) {
                    if (S & (1 << k)) {
                        int min_cost = INT_MAX;
                        // For all w in S, w != k
                        for (unsigned int w = 0; w < n; w++) {
                            if (w != k && S & (1 << w)) {
                                int cost = C[S & ~(1 << k)][w] + G[w][k];
                                if (cost < min_cost) {
                                    min_cost = cost;
                                }
                            }
                        }
                        C[S][k] = min_cost;
                    }
                }
            }
            // Compute the next set using Gosper's Hack
            unsigned int c = S & -S;
            unsigned int r = S + c;
            S = (((r ^ S) >> 2) / c) | r;
        }
    }

    // Use computed subproblems to find the optimal cost
    int opt_cost = INT_MAX;
    unsigned int S_tour = ((1 << n) - 1) & ~1;
    for (int k = 1; k < n; k++) {
        int tour_cost = C[S_tour][k] + G[k][0];
        if (tour_cost < opt_cost) {
            opt_cost = tour_cost;
        }    
    }

    // Output optimal cost
    cout << opt_cost << endl;
    
    // Free memory and return
    free(T);
    for (int i = 0; i < (1 << n); i++) {
        free(C[i]);
    }
    free(C);
    return 0;
}