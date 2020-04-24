/*  Lin-Kernighan Heuristic for the Metric TSP Problem 
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <vector>
#include <limits.h>
#include <unordered_set>
using namespace std;



int tour_cost(vector<int> &tour, int **G, int n) {
    int cost = 0;
    for (int i = 0; i < n - 1; i++) {
        cost += G[i][i + 1];
    }
    cost += G[n - 1][0];
    return cost;
}


pair<int, int> make_edge(int i, int j) {
    if (i > j) {
        return pair(j, i);
    } else {
        return pair(i, j);
    }
}


vector<int> closest(int t2i, vector<int> tour, int gain, unordered_set<pair<int, int>> broken, unordered_set<pair<int, int>> joined) {
    unordered_set<pair<int, int>> neighbors;

    
}



bool improve(vector<int> &tour, int **G, int n) {
    unordered_set<pair<int, int>> broken;

    for (int t1 : tour) {
        int around[2];
        around[0] = (t1 - 1) % n;
        around[1] = (t1 + 1) % n;
        for (int i = 0; i < 2; i++) {
            int t2 = around[i];
            broken.insert(make_edge(t1, t2));
            int gain = G[t1][t2];

        }

    }
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

    vector<int> initial_tour;
    for (int i = 0; i < n; i++) {
        initial_tour.push_back(i);
    }

    bool improved = true;
    while (improved) {
        improved = improve(initial_tour, G, n);
    }

    

    // Output optimal cost
    cout << opt_cost << endl;
    
    return 0;
}