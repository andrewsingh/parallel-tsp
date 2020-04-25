#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

pair<int, int> make_edge(int i, int j) {
    if (i > j) {
        return pair(j, i);
    } else {
        return pair(i, j);
    }
}


int get_tour_dist(vector<int> &tour, int **G) {
    int dist = 0;
    for (int i = 0; i < tour.size - 1; i++) {
        dist += G[i][i + 1];
    }
    dist += G[tour.size - 1][0];
    return dist;
}


void reverse_tour(int start, int end, vector<int> &tour) {
    int current = start;
    int next = tour[start];
    int next_next;
    do {
        next_next = tour[next];
        tour[next] = current;
        current = next;
        next = next_next;
    } while (current != end);
}

bool is_tour(vector<int> &tour, int n) {
    int count = 1;
    int start = tour[0];
    while (start != 0) {
        start = tour[start];
        count++;
    }
    return (count == n);
}


void lk_move(int tour_start, vector<int> &tour, int **G, int n) {
    set<pair<int, int>> broken_set, joined_set;
    vector<int> tour_opt = tour;
    double g_opt = 0;
    double g = 0;
    double g_local;
    int last_next_v = tour_start;
    int from_v;
    int next_v;
    int next_from_v;
    int last_possible_next_v;
    pair<int, int> broken_edge;
    double y_opt_length;
    double broken_edge_length;
    double g_opt_local;

    from_v = tour[last_next_v];
    long init_tour_dist = get_tour_dist(tour, G);

    do {
        next_v = -1;
        broken_edge = make_edge(last_next_v, from_v);
        broken_edge_length = G[broken_edge.first][broken_edge.second];

        if (joined_set.count(broken_edge) > 0) {
            break;
        }

        for (int possible_next_v = tour[from_v]; next_v == -1 && possible_next_v != tour_start; 
            possible_next_v = tour[possible_next_v]) {
            
            g_local = broken_edge_length - G[from_v][possible_next_v];

            if (!(
                broken_set.count(make_edge(from_v, possible_next_v)) == 0 &&
                g + g_local > 0 &&
                joined_set.count(make_edge(last_possible_next_v, possible_next_v)) == 0 &&
                tour[possible_next_v] != 0 &&
                possible_next_v != tour[from_v]
            )) {
                last_possible_next_v = possible_next_v;
                continue;
            }

            next_v = possible_next_v;
        }

        if (next_v != -1) {
            broken_set.insert(broken_edge);
            joined_set.insert(make_edge(from_v, next_v));

            y_opt_length = G[from_v][tour_start];
            g_opt_local = g + (broken_edge_length - y_opt_length);

            if (g_opt_local > g_opt) {
                g_opt = g_opt_local;
                tour_opt = tour;
                tour_opt[tour_start] = from_v;
            }

            g += broken_edge_length - G[from_v][next_v];
            reverse_tour(from_v, last_possible_next_v, tour);
            next_from_v = last_possible_next_v;
            tour[from_v] = next_v;
            last_next_v = next_v;
            from_v = next_from_v;
        }
    } while (next_v != -1);

    tour = tour_opt;
    long distance_after = get_tour_dist(tour, G);
    assert(distance_after <= init_tour_dist);
    assert(is_tour(tour, n));

}

void optimize_tour(vector<int> &tour, int **G, int n) {
    int diff;
    int old_dist = 0;
    int new_dist = 0;

    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < n; i++) {
            lk_move(i, tour, G, n);
        }
        new_dist = get_tour_dist(tour, G);
        diff = old_dist - new_dist;
        if (j != 0) {
            assert(diff >= 0);
            if (diff == 0) {
                break;
            }
        }
        old_dist = new_dist;
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