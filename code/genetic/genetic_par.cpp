#include <iostream>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

// Represents an individual solution
struct individual {
    vector<int> cities;
    int path_len;
    int rank;
    int offset; // used in roulette selection
};

// Represents a set of parents to crossover
struct parents {
    individual p1;
    individual p2;
};

// Represents all the individuals in the current population
struct population {
    individual *ids;
    int total_len; // total length of all individual paths
    int size;
    parents *pars;
};

// Generate a random, initial population of size n
population generate_initial(int n, int **weights) {
    population pop;
    pop.ids = (individual *)malloc(n*sizeof(individual));
    pop.pars = (parents *) malloc(n*sizeof(parents));
    int all_visited = (1 << n) - 1;

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        individual ind;
        // int visited = 0;
        vector<int> visited;
        // pick a random starting city
        int c = rand() % n;
        int oc = c;
        ind.cities.push_back(c);
        // visited |= (1 << c);
        visited.push_back(c);
        int length = 0;
        // while (visited != all_visited) {
        while (visited.size() < n) {
            // pick a random vertex that hasn't been visited yet
            vector<int> next;
            for (int j = 0; j < n; j++) {
                // theres a valid edge from c to j and j hasn't been visited yet
                // if ((((visited >> j) & 1) == 0)) {
                if (find(visited.begin(), visited.end(), j) == visited.end()) {
                    next.push_back(j);
                }
            }
            int nci = rand() % next.size();
            int nc = next[nci];
            ind.cities.push_back(nc);
            length += weights[c][nc];
            c = nc;
            // visited |= (1 << nc);
            visited.push_back(nc);
        // continue until all cities visited
        }
        length += weights[c][oc];
        ind.path_len = length;
        pop.ids[i] = ind;
        pop.total_len += length;
    }


    pop.size = n;
    return pop;
}

// Comparison function for individuals by fitness
bool ind_compare (individual i, individual j) { 
    return i.path_len > j.path_len; 
}

// After ranks assigned, do roulette selection with probabilities defined by ranks
individual roulette_selection(population pop) {
    individual last = pop.ids[pop.size - 1];
    int max = last.offset + last.rank;
    int rand_id = rand() % max;
    individual curr = pop.ids[0];
    int i = 1;
    while (curr.offset + curr.rank <= rand_id) {
        curr = pop.ids[i];
        i++;
    }
    return curr;
}

// Given population of size p, select p-1 pairs of parents for the next generation
void select_parents(population &pop) {
    sort(pop.ids, pop.ids + pop.size, ind_compare);
    // assign ranks and offsets
    int offset = 0;
    for (int i = 0; i < pop.size; i++) {
        pop.ids[i].rank = i + 1;
        pop.ids[i].offset = offset;
        offset += i + 1;
    }

    // Use roulette selection to select pairs of parents
    #pragma omp parallel for
    for (int i = 0; i < pop.size - 1; i++) {
        parents p;
        p.p1 = roulette_selection(pop);
        p.p2 = roulette_selection(pop);
        pop.pars[i] = p;
    }
}

// Given two parents, apply a greedy crossover method to create one new individaul
individual crossover(individual &p1, individual &p2, int n, int **weights) {
    individual ind;
    int all_visited = (1 << n) - 1;
    // pick a random starting city
    int c = rand() % n;
    int oc = c;
    ind.cities.push_back(c);
    // int visited = (1 << c);
    vector<int> visited;
    visited.push_back(c);
    int length = 0;
    // while (visited != all_visited) {
    while (visited.size() < n) {
        // figure out what cities (c1 and c2) come next in p1 and p2 respectively
        int ci1 = (find(p1.cities.begin(), p1.cities.end(), c)) - p1.cities.begin();
        int ci2 = (find(p2.cities.begin(), p2.cities.end(), c)) - p2.cities.begin();
        int c1 = p1.cities[(ci1 + 1) % n];
        int c2 = p2.cities[(ci2 + 1) % n];
        int w1 = weights[c][c1];
        int w2 = weights[c][c2];
        
        bool cycle = false;
        // select the closer city that doesn't create a cycle
        if (w1 < w2) {
            // if its been visited, dont visit it again
            // if (((visited >> c1) & 1) == 1) {
            if (find(visited.begin(), visited.end(), c1) != visited.end()) {
                cycle = true;
            }
            // otherwise its fine to visit it 
            else {
                ind.cities.push_back(c1);
                length += weights[c][c1];
                c = c1;
                // visited |= (1 << c1);
                visited.push_back(c1);
            }
        } else {
            // if (((visited >> c2) & 1) == 1) {
            if (find(visited.begin(), visited.end(), c2) != visited.end()) {
                cycle = true;
            } else {
                ind.cities.push_back(c2);
                length += weights[c][c2];
                c = c2;
                // visited |= (1 << c2);
                visited.push_back(c2);
            }
        }

        bool cycle1 = false;
        // try the other city if the closer one creates a cycle
        if (cycle) {
            if (w1 >= w2) {
                // if (((visited >> c1) & 1) == 1) {
                if (find(visited.begin(), visited.end(), c1) != visited.end()) {
                    cycle1 = true;
                } else {
                    ind.cities.push_back(c1);
                    length += weights[c][c1];
                    c = c1;
                    // visited |= (1 << c1);
                    visited.push_back(c1);
                }
            } else {
                // if (((visited >> c2) & 1) == 1) {
                if (find(visited.begin(), visited.end(), c2) != visited.end()) {
                    cycle1 = true;
                } else {
                    ind.cities.push_back(c2);
                    length += weights[c][c2];
                    c = c2;
                    // visited |= (1 << c2);
                    visited.push_back(c2);
                }
            }
        }

        // if both cities create a cycle, pick a random city
        if (cycle1) {
            vector<int> next;
            // pick a random vertex that hasn't been visited yet
            for (int j = 0; j < n; j++) {
                // theres a valid edge from c to j and j hasn't been visited yet
                // if ((((visited >> j) & 1) == 0)) {
                if (find(visited.begin(), visited.end(), j) == visited.end()) {
                    next.push_back(j);
                }
            }
            int nci = rand() % next.size();
            int nc = next[nci];
            ind.cities.push_back(nc);
            length += weights[c][nc];
            c = nc;
            // visited |= (1 << nc);
            visited.push_back(nc);
        }

    // repeat until all cities visited
    }
    length += weights[c][oc];
    ind.path_len = length;
    return ind;
}

// mutate individual with a 2.1% probability
void mutate(individual &i, int n) {
    if (rand() % 1000 <= 21) {
        int i1 = rand() % n;
        int i2 = rand() % n;
        int tmp = i.cities[i2];
        i.cities[i2] = i.cities[i1];
        i.cities[i1] = tmp;
    }
    return;
}

// If pop size = 1 or all individuals are the same, have converged
bool convergence(population pop, int n) {
    if (pop.size == 1) {
        return true;
    }
    // check if all individuals of a population are the same
    for (int i = 1; i < pop.size; i++) {
        individual i1 = pop.ids[i-1];
        individual i2 = pop.ids[i];
        int c1 = i1.cities[0];
        int c2 = find(i2.cities.begin(), i2.cities.end(), c1) - i2.cities.begin();
        for (int j = 0; j < n; j++) {
            if (i1.cities[(c1 + j) % n] != i2.cities[(c2 + j) % n]) {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    // set random seed 
    srand (time(NULL));

    int num_threads = omp_get_max_threads();

    // Check if thread count is passed in as a command line argument
    for (int i = 0; i < argc; i++) {
        string arg(argv[i]);
        if (arg == "-t" && i + 1 < argc) {
            num_threads = atoi(argv[i + 1]);
        }
    }

    omp_set_num_threads(num_threads);
    cout << "Running with " << num_threads << " threads" << endl;

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    int n;
    // Read in number of vertices
    cin >> n;

    // set up shortest path arrays
    int **weights;
    weights = (int **)malloc(n*sizeof(int*));
    // read in edges
    for (int i = 0; i < n; i++) {
        weights[i] = (int *)malloc(n*sizeof(int));
        for (int j = 0; j < n; j++) {
            cin >> weights[i][j];
        }
    }

    // genetic algorithm
    population pop = generate_initial(n, weights);

    while (!convergence(pop, n)) {
        // eval_fitness_pop(pop, n);
        select_parents(pop);

        pop.total_len = 0;
        #pragma omp parallel for
        for (int i = 0; i < pop.size - 1; i++) {
            parents p = pop.pars[i];
            individual ind = crossover(p.p1, p.p2, n, weights);
            mutate(ind, n);
            pop.ids[i] = ind;
        }
        pop.size -= 1;
    }

    printf("Tour cost = %d\n", pop.ids[0].path_len);

    clock_gettime(CLOCK_REALTIME, &end);
    double exec_time;
    exec_time = (end.tv_sec - start.tv_sec) * 1e9; 
    exec_time = (exec_time + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    cout << "Execution time: " << exec_time << " seconds" << endl;

    return 0;
}