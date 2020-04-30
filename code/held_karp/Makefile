all:
	g++ -o seq_hk -std=c++11 ../parse/parser.cpp held_karp_seq.cpp -lm
	g++ -o par_hk -std=c++11 -fopenmp ../parse/parser.cpp held_karp_par.cpp -lm

clean:
	rm -f seq_hk
	rm -f par_hk