#include <iostream>
#include <cstring>
#include <math.h>
#include <omp.h>
#include "../parse/parser.h"

using namespace std;

// Global variables
bool is_matrix;
int n;
vector<vector<float> > G;
vector<float> X, Y;

// Returns the distance from node i to node j
float dist(int i, int j) {
    if (is_matrix) {
        return G[i][j];
    } else {
        int dx = X[i] - X[j];
        int dy = Y[i] - Y[j];
        // Round to be consistent with official TSPLIB solutions
        return (int)(sqrt(dx * dx + dy * dy) + 0.5);
    }
}

// Subset dp to solve for shortest non simple TSP path
int dp(int S, int v, int **memo) {
    // We've already solved this subproblem
    if (memo[S][v] != -1) {
        return memo[S][v];
    }

    // Base case - only one vertex left to consider
    if ((S & ~(1 << v)) == 0) {
        memo[S][v] = dist(0, v);
        return dist(0, v);
    }
    
    // Solve subprobloem (S-v, u), where u is any vertex in S that's not v or the source x
    int minval, minprev, val;
    bool first = true;
    int c = 0;
    #pragma omp parallel for
    for (int u = 0; u < n; u++) {
        if (u != v && (S >> u & 1 == 1)) {
            val = dp(S & ~(1 << (v)), u, memo) + dist(u, v);
            if (first || minval >= val) {
                minval = val;
                minprev = u;
            }
            first = false;
        } 
    }

    // Save solution in the memo table
    memo[S][v] = minval;

    return minval;
}

int main(int argc, char *argv[]) {
    int num_threads = omp_get_max_threads();

    string file_name = "";
    for (int i = 0; i < argc; i++) {
        string arg(argv[i]);
        if (arg == "-f" && i + 1 < argc) {
            file_name = argv[i + 1];
        } else if (arg == "-t" && i + 1 < argc) {
    omp_set_num_threads(num_threads);

            num_threads = atoi(argv[i + 1]);
        }
    }

    if (file_name == "") {
        cout << "Please specify a filename by adding -f [FILE_NAME]" << endl;
        return 0;
    }

    is_matrix = (file_name.find(".mat") != string::npos);
    if (is_matrix) {
        n = parse_matrix(file_name, G);
    } else {
        n = parse_euc_2d(file_name, X, Y);
    }

    if (n > 30) {
        cout << "Please run on a smaller graph with at most 30 vertices" << endl;
        return 0;
    }

    cout << "Running with " << num_threads << " threads" << endl;
    
    unsigned S = 0; // initial set of vertices
    for (int i = 0; i < n; i++) {
        S |= (1 << i);
    }

    // Set up tables and solve tsp with subset dp
    int **memo = (int **)malloc(S * sizeof(int*));
    #pragma omp parallel for
    for (int i = 0; i < S + 1; i++) {
        memo[i] = (int*)malloc(n * sizeof(int));
        memset(memo[i], -1, n * sizeof(int));
    }

    cout << "Tour cost = " << dp(S, 0, memo) << endl;

    return 0;
}