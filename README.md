# Parallel TSP
Parallel Algorithms for the Traveling Salesperson Problem (15-418 Final Project)

## Testing an algorithm
To test one of the algorithms, for example sequential Held-Karp, first compile the source code.
```
g++ -o hkseq.out held_karp_seq.cpp
```

Then run the resulting executable on one of the TSP instances.

```
./hkseq.out < ../../instances/br17.atsp
``` 
To compile a parallel program, you'll need to add the `-fopenmp` flag. So compiling the parallel Held-Karp program would look like 
```
g++ -fopenmp -o hkpar.out held_karp_par.cpp
```
By default, the parallel programs will run with `omp_get_max_threads()` threads. To run the program with a specific number of threads, add the thread flag and thread count as command line arguments:
```
./hkpar.out < ../../instances/br17.atsp -t [THREAD_COUNT]
``` 

## Adding a TSP instance
To add an instance, simply create a file with the following two contents:
1. Number of nodes n (integer)
2. n x n matrix of edge weights (integer or float)

in that order. So the file should only have 1 + n^2 space-separated numbers. Note that the edge weights must obey the triangle inequality (metric TSP). Include the number of nodes in the file name of the instance. If the edge weights are symmetric, give the file a `.tsp` extension; if they are asymmetric, give the file a `.atsp` extension. Add the instance to the `instances` directory.


## Current TSP instances
 Instance      | n  | symmetric | opt   
 :--- |:--- |:--- |:---
 br17 | 17 | no | 39
 fri26 | 26 | yes | 937
