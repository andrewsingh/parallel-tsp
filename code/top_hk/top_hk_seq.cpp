#include <iostream>
#include <cstring>

using namespace std;

// Subset dp to solve for shortest non simple TSP path
int dp(int n, int S, int v, int **weights, int **memo) {
    // We've already solved this subproblem
    if (memo[S][v] != -1) {
        return memo[S][v];
    }

    // Base case - only one vertex left to consider
    if ((S & ~(1 << v)) == 0) {
        memo[S][v] = weights[v][0];
        return weights[0][v];
    }
    
    // Solve subprobloem (S-v, u), where u is any vertex in S that's not v or the source x
    int minval, minprev, val;
    bool first = true;
    int c = 0;
    for (int u = 0; u < n; u++) {
        if (u != v && (S >> u & 1 == 1)) {
            val = dp(n, S & ~(1 << (v)), u, weights, memo) + weights[u][v];
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

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    int n;
    cin >> n;

    // Set up shortest path and next pointer arrays
    unsigned S = 0; // initial set of vertices
    int **weights;
    weights = (int **)malloc(n*n*sizeof(int*));

    for (int i = 0; i < n; i++) {
        S |= (1 << i);
        weights[i] = (int *)malloc(n*sizeof(int));
        for (int j = 0; j < n; j++) {
            cin >> weights[i][j];
        }
    }

    // Set up tables and solve tsp with subset dp
    int **memo = (int **)malloc(S * sizeof(int*));
    for (int i = 0; i < S + 1; i++) {
        memo[i] = (int*)malloc(n * sizeof(int));
        memset(memo[i], -1, n * sizeof(int));
    }

    cout << "Tour cost = " << dp(n, S, 0, weights, memo) << endl;

    clock_gettime(CLOCK_REALTIME, &end);
    double exec_time;
    exec_time = (end.tv_sec - start.tv_sec) * 1e9; 
    exec_time = (exec_time + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    cout << "Execution time: " << exec_time << " seconds" << endl;

    return 0;
}