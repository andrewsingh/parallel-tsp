/*  Sequential Lin-Kernighan heuristic
    A tour is represented as an vector such that at a city i, the next city to
    travel to is tour[i] */


#include <vector>
#include <set>
#include <iostream>
#include <math.h>
#include <assert.h>

using namespace std;


pair<int, int> make_edge(int i, int j) {
    if (i > j) {
        return make_pair(j, i);
    } else {
        return make_pair(i, j);
    }
}


int get_tour_dist(vector<int> &tour, int **G, int n) {
    int currentIndex = 0;
    double distance = 0;
    for (int i = 0; i < n; i++) {
        distance += G[i][tour[i]];
    }

    return distance;
}

// Reverse tour from indices start to end
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
    set<pair<int, int> > broken_set, joined_set;
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
    long init_tour_dist = get_tour_dist(tour, G, n);

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

            // Criteria for the next link y_i
            if (!(
                g + g_local > 0 &&
                tour[possible_next_v] != 0 &&
                possible_next_v != tour[from_v] &&
                broken_set.count(make_edge(from_v, possible_next_v)) == 0 &&
                joined_set.count(make_edge(last_possible_next_v, possible_next_v)) == 0
            )) {
                last_possible_next_v = possible_next_v;
                continue;
            }

            next_v = possible_next_v;
        }

        // If new y_i is valid
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
    long distance_after = get_tour_dist(tour, G, n);
    assert(distance_after <= init_tour_dist);
    assert(is_tour(tour, n));

}


int optimize_tour(vector<int> &tour, int **G, int n) {
    int diff;
    int old_dist = 0;
    int new_dist = 0;

    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < n; i++) {
            lk_move(i, tour, G, n);
        }
        new_dist = get_tour_dist(tour, G, n);
        diff = old_dist - new_dist;
        if (j != 0) {
            assert(diff >= 0);
            if (diff == 0) {
                break;
            }
        }
        old_dist = new_dist;
    }
    return new_dist;
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

    vector<int> tour = vector<int>(n, 0);

    // Initial tour
    for (int i = 0; i < n; i++) {
        tour[i] = (i + 1) % n;
    }

    int opt_cost = optimize_tour(tour, G, n);
     
    // Output optimal cost
    cout << opt_cost << endl;
    
    return 0;
}