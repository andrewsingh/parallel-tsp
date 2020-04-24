#include <iostream>
#include <stdlib.h>
using namespace std;


int main() {
    // n = number of nodes
    int n;
    cin >> n;

    float X[n];
    float Y[n];

    // Allocate weights matrix
    float G[n][n];

    // Read in lower triangular matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i + 1; j++) {
            cin >> G[i][j];
            if (i != j) {
                G[j][i] = G[i][j];
            }
        }
    }

    // Output n and distance matrix
    cout << n << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << G[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}