all:
	/opt/rh/devtoolset-9/root/bin/g++ -o seq_genetic -std=c++17 ../parse/parser.cpp genetic_seq.cpp -lm
	/opt/rh/devtoolset-9/root/bin/g++ -o par_genetic -std=c++17 -fopenmp ../parse/parser.cpp genetic_par.cpp -lm

clean:
	rm -f seq_genetic
	rm -f par_genetic