/*  Lin-Kernighan Heuristic for the Metric TSP Problem 
    Input: the number of cities n followed by the full matrix of distances.
    Output: The cost of the optimal tour.
*/
#include <iostream>
#include <vector>
#include <limits.h>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <functional>
using namespace std;



pair<int, int> make_edge(int i, int j) {
    if (i > j) {
        return pair(j, i);
    } else {
        return pair(i, j);
    }
}


int tour_cost(vector<int> &tour, int **G, int n) {
    int cost = 0;
    for (int i = 0; i < n - 1; i++) {
        cost += G[i][i + 1];
    }
    cost += G[n - 1][0];
    return cost;
}

unordered_set<pair<int, int>> get_tour_edges(vector<int> &tour) {
    unordered_set<pair<int, int>> edges;
    for (int i = 0; i < tour.size - 1; i++) {
        edges.insert(make_edge(tour[i], tour[i + 1]));
    }
    edges.insert(make_edge(tour[tour.size - 1], tour[0]));
    return edges;
}


// bool has_node(unordered_set<pair<int, int>> edge_set, int node) {
//     for (pair<int, int> e : edge_set) {
//         if (e.first == node || e.second == node) {
//             return true;
//         }
//     }
//     return false;
// }
pair<int, int> around(int node, int n) {
    return pair((node - 1) % n, (node + 1) % n);
}

vector<int> generate(vector<int> &tour, unordered_set<pair<int, int>> broken, unordered_set<pair<int, int>> joined) {
    unordered_set<pair<int, int>> edges;
    unordered_set<pair<int, int>> tour_edges = get_tour_edges(tour);
    for (auto it = tour_edges.begin(); it != tour_edges.end(); ++it) {
        if (broken.count(*it) == 0 || joined.count(*it)) {
            edges.insert(*it);
        }
    }

    if (edges.size < tour.size) {
        return vector<int>();
    }

    unordered_map<int, int> successors;
    int node = 0;
    while (edges.size > 0) {
        for (auto edge : edges) {
            int i = edge.first;
            int j = edge.second;
            if (i == node) {
                successors.at(node) = j;
                node = j;
                edges.erase(edge);
                break;
            } else if (j == node) {
                successors.at(node) = i;
                node = i;
                edges.erase(edge);
                break;
            }
        }
    }

    if (successors.size < tour.size) {
        return vector<int>();
    }
    auto succ = successors.at(0);
    vector<int> new_tour;
    new_tour.push_back(0);
    unordered_set<int> visited;
    visited.insert(0);
    while (visited.count(succ) == 0) {
        visited.insert(succ);
        new_tour.push_back(succ);
        succ = successors.at(succ);
    }

    if (new_tour.size == tour.size) {
        return new_tour;
    } else {
        return vector<int>();
    }

}



bool has_edge(vector<int> &tour, pair<int, int> edge) {
    for (int i = 0; i < tour.size - 1; i++) {
        if ((tour[i] == edge.first && tour[i + 1] == edge.second) || 
            tour[i] == edge.second && tour[i + 1] == edge.first) {
            return true;
        }
    }
    return false;
}





vector<pair<int, pair<int, int>>> closest(int t2i, vector<int> &tour, int **G, int n, 
    int gain, unordered_set<pair<int, int>> broken, unordered_set<pair<int, int>> joined) {

    unordered_map<int, pair<int, int>> neighbors;
    for (int node : tour) {
        if (node != t2i && G[t2i][node] > 0) {
            pair<int, int> yi = make_edge(t2i, node);
            int gi = gain - G[t2i][node];
            if (gi <= 0 || broken.count(yi) || has_edge(tour, yi)) {
                continue;
            }

            pair<int, int> around_node = around(node, n);
            for (int succ : {around_node.first, around_node.second}) {
                pair<int, int> xi = make_edge(node, succ);
                if (broken.count(xi) == 0 && joined.count(xi) == 0) {
                    int diff = G[node][succ] - G[t2i][node];
                    if (neighbors.count(node) && diff > neighbors.at(node).first) {
                        neighbors.at(node).first = diff;
                    } else {
                        neighbors.at(node) = pair(diff, gi);
                    }
                }
            }
        }
    }
    vector<pair<int, pair<int, int>>> neighbors_vec;
    copy(neighbors.begin(), neighbors.end(), back_inserter(neighbors_vec));
    sort(neighbors_vec.begin(), neighbors_vec.end(), [](const pair<int, pair<int, int>>& l, 
        const pair<int, pair<int, int>>& r) {
            return l.second.first < r.second.first;
    });
    return neighbors_vec;
}



bool chooseX(vector<int> &tour, int t1, int last, int **G, int n, int gain, 
    unordered_set<pair<int, int>> broken, unordered_set<pair<int, int>> joined) {
        vector<int> around_vec = {};
        pair<int, int> around_last = around(last, n);
        int pred = around_last.first;
        int succ = around_last.second;
        if (broken.size == 4) {
            if (G[pred][last] > G[succ][last]) {
                around_vec.push_back(pred);
            } else {
                around_vec.push_back(succ);
            }
        } else {
            around_vec.push_back(pred);
            around_vec.push_back(succ);
        }

        for (int t2i : around_vec) {
            auto xi = make_edge(last, t2i);
            int gi = gain + G[last][t2i];

            if (joined.count(xi) == 0 && broken.count(xi) == 0) {
                unordered_set<pair<int, int>> added;
                unordered_set<pair<int, int>> removed;
                copy(joined.begin(), joined.end(), added.begin());
                copy(broken.begin(), broken.end(), removed.begin());

                removed.insert(xi);
                added.insert(make_edge(t2i, t1));
                int relink = gi - G[t2i][t1];

            }
        }
    }




bool improve(vector<int> &tour, int **G, int n) {
    unordered_set<pair<int, int>> broken;

    for (int t1 : tour) {

        pair<int, int> around_t1 = around(t1, n);
        for (int t2 : {around_t1.first, around_t1.second}) {
            broken.insert(make_edge(t1, t2));
            int gain = G[t1][t2];
            unordered_set<pair<int, int>> joined;
            vector<pair<int, pair<int, int>>> close = closest(t2, tour, G, n, gain, broken, joined);
            int tries = 5;
            for (auto elem : close) {
                int t3 = elem.first;
                int gi = elem.second.second;
                if (around_t1.first == t3 || around_t1.second == t3) {
                    continue;
                }
                joined.insert(make_edge(t2, t3));

            }

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