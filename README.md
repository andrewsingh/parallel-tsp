# Parallel Algorithms for the Traveling Salesperson Problem
Final project for Carnegie Mellon's 15-418 Parallel Computer Architecture and Programming class. [Project report](https://github.com/andrewsingh/parallel-tsp/blob/master/15_418_Project_Final_Report.pdf).

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
