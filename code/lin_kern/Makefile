all:
	/opt/rh/devtoolset-9/root/bin/g++ -o seq_lin_kern -std=c++17 ../parse/parser.cpp lin_kern_seq.cpp -lm
	/opt/rh/devtoolset-9/root/bin/g++ -o par_lin_kern -std=c++17 -fopenmp ../parse/parser.cpp lin_kern_par.cpp -lm

clean:
	rm -f seq_lin_kern
	rm -f par_lin_kern