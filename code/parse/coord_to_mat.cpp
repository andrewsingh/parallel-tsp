#include <iostream>
#include <stdlib.h>
#include <math.h>
using namespace std;


int main() {
    // n = number of points
    int n;
    cin >> n;

    float X[n];
    float Y[n];

    // Read in coordinates of n points
    for (int i = 0; i < n; i++) {
        cin >> X[i];
        cin >> Y[i];
    }

    // Allocate weights matrix
    float G[n][n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                // A point is distance 0 away from itself
                G[i][j] = 0;
            } else {
                // Compute Euclidean distance between points i and j
                G[i][j] = sqrt(pow(X[j] - X[i], 2) + pow(Y[j] - Y[i], 2));
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