all:
	/opt/rh/devtoolset-9/root/bin/g++ -o seq_top_hk -std=c++17 ../parse/parser.cpp top_hk_seq.cpp -lm
	/opt/rh/devtoolset-9/root/bin/g++ -o par_top_hk -std=c++17 -fopenmp ../parse/parser.cpp top_hk_par.cpp -lm

clean:
	rm -f seq_top_hk
	rm -f par_top_hk