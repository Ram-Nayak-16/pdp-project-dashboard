#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <omp.h>
#endif

using namespace std;

// Structure to pass data to threads (for Windows fallback)
struct ThreadData {
    const vector<vector<double>>* A;
    const vector<vector<double>>* B;
    vector<vector<double>>* C;
    int size;
    int start_row;
    int end_row;
};

#ifdef _WIN32
DWORD WINAPI multiplyWorkerWin(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->size; j++) {
            double sum = 0;
            for (int k = 0; k < data->size; k++) {
                sum += (*(data->A))[i][k] * (*(data->B))[k][j];
            }
            (*(data->C))[i][j] = sum;
        }
    }
    return 0;
}
#endif

// Function to initialize matrix with random values
void initializeMatrix(vector<vector<double>>& matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (double)(rand() % 10);
        }
    }
}

// Sequential Matrix Multiplication
void multiplySequential(const vector<vector<double>>& A, 
                        const vector<vector<double>>& B, 
                        vector<vector<double>>& C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            C[i][j] = 0;
            for (int k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Parallel Matrix Multiplication (Platform-Agnostic)
void multiplyParallel(const vector<vector<double>>& A, 
                      const vector<vector<double>>& B, 
                      vector<vector<double>>& C, int size) {
#ifdef _WIN32
    // Windows Native Threads Fallback
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_threads = sysinfo.dwNumberOfProcessors;
    if (num_threads == 0) num_threads = 2;

    HANDLE* threads = new HANDLE[num_threads];
    ThreadData* data = new ThreadData[num_threads];
    int rows_per_thread = size / num_threads;

    for (int i = 0; i < num_threads; i++) {
        data[i].A = &A; data[i].B = &B; data[i].C = &C;
        data[i].size = size;
        data[i].start_row = i * rows_per_thread;
        data[i].end_row = (i == num_threads - 1) ? size : (i + 1) * rows_per_thread;
        threads[i] = CreateThread(NULL, 0, multiplyWorkerWin, &data[i], 0, NULL);
    }
    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);
    for (int i = 0; i < num_threads; i++) CloseHandle(threads[i]);
    delete[] threads; delete[] data;
#else
    // OpenMP for Linux/Cloud/Modern environments
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double sum = 0;
            for (int k = 0; k < size; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
#endif
}

// Function to verify if two matrices are equal
bool verify(const vector<vector<double>>& C1, 
            const vector<vector<double>>& C2, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (fabs(C1[i][j] - C2[i][j]) > 1e-9) return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    int size;
    cout << "--------------------------------------------------" << endl;
    cout << "  Parallel Matrix Multiplication (Universal) " << endl;
    cout << "--------------------------------------------------" << endl;
    
    if (argc > 1) {
        size = atoi(argv[1]);
    } else {
        cout << "Enter size: ";
        if (!(cin >> size) || size <= 0) return 1;
    }

    vector<vector<double>> A(size, vector<double>(size));
    vector<vector<double>> B(size, vector<double>(size));
    vector<vector<double>> C_seq(size, vector<double>(size));
    vector<vector<double>> C_par(size, vector<double>(size));

    srand(time(0));
    initializeMatrix(A, size);
    initializeMatrix(B, size);

    auto start_seq = chrono::high_resolution_clock::now();
    multiplySequential(A, B, C_seq, size);
    auto end_seq = chrono::high_resolution_clock::now();
    chrono::duration<double> time_seq = end_seq - start_seq;
    cout << "Sequential Time: " << fixed << setprecision(4) << time_seq.count() << " seconds" << endl;

    auto start_par = chrono::high_resolution_clock::now();
    multiplyParallel(A, B, C_par, size);
    auto end_par = chrono::high_resolution_clock::now();
    chrono::duration<double> time_par = end_par - start_par;
    cout << "Parallel Time:   " << fixed << setprecision(4) << time_par.count() << " seconds" << endl;

    if (verify(C_seq, C_par, size)) {
        cout << "Verification: SUCCESS" << endl;
    } else {
        cout << "Verification: FAILED" << endl;
    }

    double speedup = time_seq.count() / time_par.count();
    int cores;
#ifdef _WIN32
    SYSTEM_INFO si; GetSystemInfo(&si); cores = si.dwNumberOfProcessors;
#else
    cores = omp_get_max_threads();
#endif
    
    cout << "Speedup:      " << speedup << "x" << endl;
    cout << "Efficiency:   " << (speedup / cores) * 100 << "%" << endl;
    cout << "Cores Used:   " << cores << endl;
    cout << "--------------------------------------------------" << endl;

    return 0;
}
