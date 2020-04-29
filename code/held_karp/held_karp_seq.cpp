/*  Sequential Held-Karp Algorithm for the Metric TSP Problem 
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <float.h>
#include "../parse/parser.h"

using namespace std;


// Global variables
int n;
vector<vector<float> > G;


int main(int argc, char *argv[]) {
    string file_name = "";
    for (int i = 0; i < argc; i++) {
        string arg(argv[i]);
        if (arg == "-f" && i + 1 < argc) {
            file_name = argv[i + 1];
        }
    }

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
            // Compute the next set using Gosper's Hack
            unsigned int c = S & -S;
            unsigned int r = S + c;
            S = (((r ^ S) >> 2) / c) | r;
        }
    }

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