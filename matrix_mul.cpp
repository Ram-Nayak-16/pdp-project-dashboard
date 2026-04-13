#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

#ifdef _OPENMP
    #include <omp.h>
#else
    #ifdef _WIN32
        #include <windows.h>
    #endif
#endif

using namespace std;

// Threshold for parallel execution (overhead of threads vs computation)
const int PARALLEL_THRESHOLD = 250;

// Structure to pass data to threads (for Windows fallback)
struct ThreadData {
    const vector<double>* A;
    const vector<double>* B;
    vector<double>* C;
    int size;
    int start_row;
    int end_row;
};

#ifdef _WIN32
DWORD WINAPI multiplyWorkerWin(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    int size = data->size;
    const vector<double>& A = *(data->A);
    const vector<double>& B = *(data->B);
    vector<double>& C = *(data->C);

    // Optimized IKJ order for cache efficiency even in thread workers
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int k = 0; k < size; k++) {
            double temp = A[i * size + k];
            for (int j = 0; j < size; j++) {
                C[i * size + j] += temp * B[k * size + j];
            }
        }
    }
    return 0;
}
#endif

// Function to initialize matrix with random values (Flat implementation)
void initializeMatrix(vector<double>& matrix, int size) {
    for (int i = 0; i < size * size; i++) {
        matrix[i] = (double)(rand() % 10);
    }
}

// Optimized Sequential Matrix Multiplication (IKJ Order)
void multiplySequential(const vector<double>& A, 
                        const vector<double>& B, 
                        vector<double>& C, int size) {
    // Zero out C
    fill(C.begin(), C.end(), 0.0);
    
    // IKJ order: Best for cache because innermost loop accesses B and C row-wise (contiguously)
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < size; k++) {
            double temp = A[i * size + k];
            for (int j = 0; j < size; j++) {
                C[i * size + j] += temp * B[k * size + j];
            }
        }
    }
}

// Optimized Parallel Matrix Multiplication (IKJ + OpenMP/Threads)
void multiplyParallel(const vector<double>& A, 
                      const vector<double>& B, 
                      vector<double>& C, int size) {
    // Zero out C
    fill(C.begin(), C.end(), 0.0);

    if (size < PARALLEL_THRESHOLD) {
        multiplySequential(A, B, C, size);
        return;
    }

#ifdef _OPENMP
    // Modern OpenMP Implementation
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < size; k++) {
            double temp = A[i * size + k];
            for (int j = 0; j < size; j++) {
                C[i * size + j] += temp * B[k * size + j];
            }
        }
    }
#elif defined(_WIN32)
    // Windows Native Threads Fallback
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int num_threads = sysinfo.dwNumberOfProcessors;
    if (num_threads <= 0) num_threads = 2;

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
    // Generic fallback
    multiplySequential(A, B, C, size);
#endif
}

// Function to verify if two matrices are equal
bool verify(const vector<double>& C1, 
            const vector<double>& C2, int size) {
    for (int i = 0; i < size * size; i++) {
        if (fabs(C1[i] - C2[i]) > 1e-9) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    int size;
    cout << "--------------------------------------------------" << endl;
    cout << "  Optimized Parallel Matrix Multiplication " << endl;
    cout << "--------------------------------------------------" << endl;
    
    if (argc > 1) {
        size = atoi(argv[1]);
    } else {
        cout << "Enter size: ";
        if (!(cin >> size) || size <= 0) return 1;
    }

    vector<double> A(size * size);
    vector<double> B(size * size);
    vector<double> C_seq(size * size);
    vector<double> C_par(size * size);

    srand(time(0));
    initializeMatrix(A, size);
    initializeMatrix(B, size);

    // Timing Sequential
    auto start_seq = chrono::high_resolution_clock::now();
    multiplySequential(A, B, C_seq, size);
    auto end_seq = chrono::high_resolution_clock::now();
    chrono::duration<double> time_seq = end_seq - start_seq;
    cout << "Sequential Time: " << fixed << setprecision(4) << time_seq.count() << " seconds" << endl;

    // Timing Parallel
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
#ifdef _OPENMP
    cores = omp_get_max_threads();
#elif defined(_WIN32)
    SYSTEM_INFO si; GetSystemInfo(&si); cores = si.dwNumberOfProcessors;
#else
    cores = 1;
#endif
    
    cout << "Speedup:      " << speedup << "x" << endl;
    cout << "Efficiency:   " << (speedup / cores) * 100 << "%" << endl;
    cout << "Cores Used:   " << cores << endl;
    cout << "--------------------------------------------------" << endl;

    return 0;
}
