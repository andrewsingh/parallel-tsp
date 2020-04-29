/*  Parallel Lin-Kernighan heuristic */


#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <random>
#include <stdlib.h>
#include <float.h>
#include <assert.h>
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


// Returns the sorted edge between nodes i and j
pair<int, int> make_edge(int i, int j) {
    if (i > j) {
        return make_pair(j, i);
    } else {
        return make_pair(i, j);
    }
}


// Returns the total distance of tour
int get_tour_dist(vector<int> &tour) {
    int currentIndex = 0;
    double distance = 0;
    for (int i = 0; i < n; i++) {
        distance += dist(i, tour[i]);
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


// Check if tour is valid
bool is_tour(vector<int> &tour) {
    int count = 1;
    int start = tour[0];
    while (start != 0) {
        start = tour[start];
        count++;
    }
    return (count == n);
}


// A single step of Lin-Kernighan
void lk_move(int tour_start, vector<int> &tour) {
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
    long init_tour_dist = get_tour_dist(tour);

    do {
        next_v = -1;
        broken_edge = make_edge(last_next_v, from_v);
        broken_edge_length = dist(broken_edge.first, broken_edge.second);

        if (joined_set.count(broken_edge) > 0) {
            break;
        }

        for (int possible_next_v = tour[from_v]; next_v == -1 && possible_next_v != tour_start; 
            possible_next_v = tour[possible_next_v]) {
            
            g_local = broken_edge_length - dist(from_v, possible_next_v);

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

            y_opt_length = dist(from_v, tour_start);
            g_opt_local = g + (broken_edge_length - y_opt_length);

            if (g_opt_local > g_opt) {
                g_opt = g_opt_local;
                tour_opt = tour;
                tour_opt[tour_start] = from_v;
            }

            g += broken_edge_length - dist(from_v, next_v);
            reverse_tour(from_v, last_possible_next_v, tour);
            next_from_v = last_possible_next_v;
            tour[from_v] = next_v;
            last_next_v = next_v;
            from_v = next_from_v;
        }
    } while (next_v != -1);

    tour = tour_opt;
    long distance_after = get_tour_dist(tour);
    //assert(distance_after <= init_tour_dist);
}


/* A single run of the Lin-Kernighan algorithm with a random initial tour
    A tour is represented as an vector such that at city i, the next city to
    travel to is tour[i] */
int lin_kernighan(int seed) {
    int diff;
    int old_dist = 0;
    int new_dist = 0;

    vector<int> perm = vector<int>(n, 0);
    for (int i = 0; i < n; i++) {
        perm[i] = i;
    }
    shuffle(perm.begin(), perm.end(), default_random_engine(seed));
    vector<int> tour = vector<int>(n, 0);
    for (int i = 0; i < n - 1; i++) {
        tour[perm[i]] = perm[i + 1];
    }
    tour[perm[n - 1]] = perm[0];
    
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < n; i++) {
            lk_move(i, tour);
        }
        new_dist = get_tour_dist(tour);
        diff = old_dist - new_dist;
        if (j != 0) {
            assert(diff >= 0);
            if (diff == 0) {
                break;
            }
        }
        old_dist = new_dist;
    }

    assert(is_tour(tour));
    return new_dist;
}
 


int main(int argc, char *argv[]) {
    string file_name = "";
    int runs = 0;
    int max_threads = omp_get_max_threads();
    int num_threads = max_threads;
    for (int i = 0; i < argc; i++) {
        string arg(argv[i]);
        if (arg == "-f" && i + 1 < argc) {
            file_name = argv[i + 1];
        } else if (arg == "-r" && i + 1 < argc) {
            runs = atoi(argv[i + 1]);
        } else if (arg == "-t" && i + 1 < argc) {
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

    if (runs == 0) {
        runs = ceil((1080 * exp(-0.008 * n)) / (double)max_threads) * (double)max_threads;
    }

    omp_set_num_threads(num_threads);
    cout << "Running with " << num_threads << " threads" << endl;
    cout << runs << " runs" << endl;

    // Run Lin-Kernighan 'runs' times and output the lowest cost
    float cost;
    float opt_cost = FLT_MAX;
    #pragma omp parallel 
    {
        int thread_num = omp_get_thread_num();
        #pragma omp for schedule(static) reduction(min:opt_cost)
        for (int i = 0; i < runs; i++) {
            cost = lin_kernighan((thread_num + 1) * (i + 1));
            if (cost < opt_cost) {
                opt_cost = cost;
            }
        }
    }
    
    // Output optimal cost
    cout << "Tour cost = " << opt_cost << endl;
    return 0;
}