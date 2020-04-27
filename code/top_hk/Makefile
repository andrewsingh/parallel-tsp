all:
	g++ -o seq_top_hk -std=c++11 top_hk_seq.cpp -lm
	g++ -o par_top_hk -std=c++11 -fopenmp top_hk_par.cpp -lm

clean:
	rm -f seq_top_hk