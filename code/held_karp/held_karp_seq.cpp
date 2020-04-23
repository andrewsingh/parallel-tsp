/*  Sequential Held-Karp Algorithm for the Metric TSP Problem 
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
using namespace std;


int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

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

    // First step of Held-Karp: compute base cases
    for (int k = 1; k < n; k++) {
        C[1 << k][k] = G[0][k];
    }

    // Main loop of Held-Karp: compute all subproblems via bottom-up DP
    for (int p = 2; p < n; p++) {
        unsigned int S = (1 << p) - 1;
        int limit = 1 << n;
        // For all S a subset of {1, 2, ..., n - 1} such that |S| = p
        while (S < limit) {
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
    cout << "Tour cost = " << opt_cost << endl;
    
    // Free memory and return
    for (int i = 0; i < (1 << n); i++) {
        free(C[i]);
    }
    free(C);

    clock_gettime(CLOCK_REALTIME, &end);
    double exec_time;
    exec_time = (end.tv_sec - start.tv_sec) * 1e9; 
    exec_time = (exec_time + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    cout << "Execution time: " << exec_time << " seconds" << endl;
    return 0;
}