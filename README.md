# Parallel TSP
Parallel Algorithms for the Traveling Salesperson Problem (15-418 Final Project)

## Testing an algorithm
To test one of the algorithms, for example Held-Karp, compile the code: `g++ held_karp.cpp`. Then run the resulting executable on one of the TSP instances: `./a.out < ../instances/br17.atsp`.

## Adding a TSP instance
To add an instance, simply create a file with the following two contents:
1. Number of nodes n
2. n x n matrix of edge weights

in that order. So the file should only have 1 + n^2 space-separated numbers. Note that the edge weights must obey the triangle inequality (metric TSP). Add the instance to the `instances` directory.


## Current TSP instances
 Instance      | n  | symmetric | opt   
 :--- |:--- |:--- |:---
 br17 | 17 | no | 39
 fri26 | 26 | yes | 937
