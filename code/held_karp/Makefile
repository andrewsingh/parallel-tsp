all:
	/opt/rh/devtoolset-9/root/bin/g++ -o seq_hk -std=c++17 ../parse/parser.cpp held_karp_seq.cpp -lm
	/opt/rh/devtoolset-9/root/bin/g++ -o par_hk -std=c++17 -fopenmp ../parse/parser.cpp held_karp_par.cpp -lm

clean:
	rm -f seq_hk
	rm -f par_hk