#include "parser.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


// Parses distance matrix of file_name into G and returns number of nodes n
int parse_matrix(string file_name, vector<vector<float> > &G) {
    ifstream instance;
    string str;
    int n;

    instance.open("../../instances/matrix/" + file_name, ios::in);
    instance >> n;
    for (int i = 0; i < n; i++) {
        G.push_back(vector<float>(n, 0));
        for (int j = 0; j < n; j++) {
            instance >> G[i][j];
        }
    }
    return n;
}


// Parses coordinates of file_name into X and Y and returns number of nodes n
int parse_euc_2d(string file_name, vector<float> &X, vector<float> &Y) {
    ifstream instance;
    string str;
    int n;
    float xi, yi;

    instance.open("../../instances/euc2d/" + file_name, ios::in);
    instance >> str;
    while (instance.good() && str != "DIMENSION" && str != "DIMENSION:") {
        instance >> str;
    }
    if (str == "DIMENSION") {
        instance >> str;
    }
    instance >> n;
    while (str != "NODE_COORD_SECTION") {
        instance >> str;
    }
    for (int i = 0; i < n; i++) {
        instance >> str;
        instance >> xi;
        instance >> yi;
        X.push_back(xi);
        Y.push_back(yi);
    }
    assert(X.size() == n);
    assert(Y.size() == n);
    return n;
}
