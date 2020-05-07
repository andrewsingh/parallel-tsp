# Parallel TSP
Parallel Algorithms for the Traveling Salesperson Problem (15-418 Final Project)

## Testing an algorithm
To test one of the algorithms, first run the make script.
```
./makeall.sh
```

Then run the algorithm through the benchmark script.
```
python3 ./benchmark.py -args
``` 
Running the benchmark script with the `-h` flag lists the different command line options for benchmarking.

## References
1. The Lin-Kernighan implementation in `code/lin_kern/lin_kern.cpp` refers to code from https://github.com/lingz/LK-Heuristic.
1. The TSP instances were downloaded from the TSPLIB website http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/.
