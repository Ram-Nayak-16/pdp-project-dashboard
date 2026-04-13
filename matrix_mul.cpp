#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

#ifdef _OPENMP
    #include <omp.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

const int PARALLEL_THRESHOLD = 1200;

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

    const int BLOCK_SIZE = 64; 

    for (int i = data->start_row; i < data->end_row; i += BLOCK_SIZE) {
        for (int k = 0; k < size; k += BLOCK_SIZE) {
            for (int j = 0; j < size; j += BLOCK_SIZE) {
                int i_end = min(i + BLOCK_SIZE, data->end_row);
                int k_end = min(k + BLOCK_SIZE, size);
                int j_end = min(j + BLOCK_SIZE, size);

                for (int ii = i; ii < i_end; ii++) {
                    for (int kk = k; kk < k_end; kk++) {
                        double temp = A[ii * size + kk];
                        for (int jj = j; jj < j_end; jj++) {
                            C[ii * size + jj] += temp * B[kk * size + jj];
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#endif

void initializeMatrix(vector<double>& matrix, int size) {
    for (int i = 0; i < size * size; i++) {
        matrix[i] = (double)(rand() % 10);
    }
}

void multiplySequential(const vector<double>& A, 
                        const vector<double>& B, 
                        vector<double>& C, int size) {
    fill(C.begin(), C.end(), 0.0);
    
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < size; k++) {
            double temp = A[i * size + k];
            for (int j = 0; j < size; j++) {
                C[i * size + j] += temp * B[k * size + j];
            }
        }
    }
}

const int THRESHOLD_LIGHT = 800;   
const int THRESHOLD_HEAVY = 1500;  

void multiplyParallel(const vector<double>& A, 
                      const vector<double>& B, 
                      vector<double>& C, int size) {
    fill(C.begin(), C.end(), 0.0);

    if (size < THRESHOLD_LIGHT) {
        cout << "Mode: [SEQUENTIAL-LIGHT]" << endl;
        multiplySequential(A, B, C, size);
        return;
    }

#ifdef _OPENMP
    int threads = 2;
    int block_size = 32;
    string mode_name = "LIGHT-PARALLEL";

    if (size >= THRESHOLD_HEAVY) {
        threads = 4;
        block_size = 64;
        mode_name = "HEAVY-PARALLEL (SIMD)";
    }

    int system_max = omp_get_max_threads();
    if (threads > system_max) threads = system_max;
    
    cout << "Mode: [" << mode_name << "] Threads: " << threads << endl;

    #pragma omp parallel for num_threads(threads) schedule(static)
    for (int i = 0; i < size; i += block_size) {
        for (int k = 0; k < size; k += block_size) {
            for (int j = 0; j < size; j += block_size) {
                int i_end = min(i + block_size, size);
                int k_end = min(k + block_size, size);
                int j_end = min(j + block_size, size);

                for (int ii = i; ii < i_end; ii++) {
                    for (int kk = k; kk < k_end; kk++) {
                        double temp = A[ii * size + kk];
                        #pragma omp simd
                        for (int jj = j; jj < j_end; jj++) {
                            C[ii * size + jj] += temp * B[kk * size + jj];
                        }
                    }
                }
            }
        }
    }
#elif defined(_WIN32)
    cout << "Mode: [WINDOWS-NATIVE] Tiled" << endl;
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
    multiplySequential(A, B, C, size);
#endif
}

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
