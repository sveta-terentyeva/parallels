#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

static const int N = 1000;   // Matrix size
static const int THREADS = 8; // Number of threads

// Function to fill the matrix with random numbers
void fillMatrix(vector<vector<int>>& matrix) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = dist(gen);
        }
    }
}

// Sequential row swapping function
void swapRowsSequential(vector<vector<int>>& matrix) {
    for (int i = 0; i < N - 1; i += 2) {
        for (int j = 0; j < N; ++j) {
            swap(matrix[i][j], matrix[i + 1][j]);
        }
    }
}

// Helper function called by each thread:
// swaps row pairs in the range [startRow; endRow)
void swapRowsPart(vector<vector<int>>& matrix, int startRow, int endRow) {
    for (int i = startRow; i < endRow - 1; i += 2) {
        for (int j = 0; j < N; ++j) {
            swap(matrix[i][j], matrix[i + 1][j]);
        }
    }
}

// Parallel row swapping function
void swapRowsParallel(vector<vector<int>>& matrix) {
    vector<thread> threads;
    threads.reserve(THREADS);

    // Number of row pairs is approximately N/2
    int pairsCount = N / 2;
    // Number of row pairs per thread
    int chunkSize = (pairsCount + THREADS - 1) / THREADS;

    for (int t = 0; t < THREADS; ++t) {
        // Start index for rows
        int start = t * chunkSize * 2;
        // End index (non-inclusive)
        int end = (t + 1) * chunkSize * 2;

        // Ensure we don't go beyond the last row (the last thread may get fewer rows)
        if (end > N) {
            end = N;
        }

        // Start a thread to process rows in the range [start; end)
        threads.emplace_back(swapRowsPart, ref(matrix), start, end);
    }

    // Wait for all threads to finish
    for (auto& thr : threads) {
        thr.join();
    }
}

int main() {
    // Create a matrix (N x N)
    vector<vector<int>> matrix(N, vector<int>(N));
    cout << "Matrix size: " << N << "\n\n";
    
    // ----------------------------
    // Sequential version
    // ----------------------------
    fillMatrix(matrix);
    auto startSeq = chrono::high_resolution_clock::now();
    swapRowsSequential(matrix);
    auto endSeq = chrono::high_resolution_clock::now();
    double seqTime = chrono::duration<double>(endSeq - startSeq).count();

    cout << "Without parallelization:\n";
    cout << "Time: " << seqTime << " s\n\n";

    // ----------------------------
    // Parallel version
    // ----------------------------
    fillMatrix(matrix);
    auto startPar = chrono::high_resolution_clock::now();
    swapRowsParallel(matrix);
    auto endPar = chrono::high_resolution_clock::now();
    double parTime = chrono::duration<double>(endPar - startPar).count();

    cout << "With parallelization (" << THREADS << " threads):\n";
    cout << "Time: " << parTime << " s\n\n";

    return 0;
}
