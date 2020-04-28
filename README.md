# Parallel TSP
Parallel Algorithms for the Traveling Salesperson Problem (15-418 Final Project)

## Testing an algorithm
To test one of the algorithms, for example the sequential Held-Karp algorithm, compile the code.
```
g++ -o hkseq.out held_karp_seq.cpp
```

Then run the resulting executable on one of the TSP instances.

```
./hkseq.out < ../../instances/br17.atsp
``` 
To compile a parallel algorithm, you'll need to add the `-fopenmp` flag. So compiling the parallel Held-Karp algorithm would look like 
```
g++ -fopenmp -o hkpar.out held_karp_par.cpp
```

## Adding a TSP instance
To add an instance, simply create a file with the following 3 contents:
1. Instance type (either `MATRIX` or `EUC_2D`)
2. Number of nodes n
3. If `MATRIX` instance, then the n x n matrix of edge weights. If `EUC_2D` instance, then a list of the (x, y) coordinates for each node. 

in that order. So the file should only contain the instance type followed by either 1 + n^2 (`MATRIX`) or 1 + 2n (`EUC_2D`) space-separated numbers. The numbers can be either integers or floats. Note that for `MATRIX` instances, the edge weights must obey the triangle inequality (metric TSP). Include the number of nodes in the file name of the instance. If the edge weights are symmetric, give the file a `.tsp` extension; if they are asymmetric, give the file a `.atsp` extension. Add the instance to the `instances` directory.


## Current TSP instances
 Instance      | n  | symmetric | opt   
 :--- |:--- |:--- |:---
 br17 | 17 | no | 39
 fri26 | 26 | yes | 937
 gr21 | 21 | yes | 2707
 gr24 | 24 | yes | 1272
 lin105 | 105 | yes | 14379
 lin318 | 318 | yes | 42029
 u1060 | 1060 | yes | 224094
 u2319 | 2319 | yes | 234256
