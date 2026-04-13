<div align="center">
  <h1>PROJECT REPORT</h1>
  <br />
  <img src="https://img.icons8.com/color/96/000000/matrix.png" width="100" />
  <h2>Parallel Matrix Multiplication using Portable C++ Threads (C++11)</h2>
  <hr />
  <br />
  
  <strong>Course:</strong> Parallel Computation and Distributed System<br />
  <strong>Department:</strong> Computer Science and Engineering (CSE)<br />
  
  <br /><br />
  
  <h3>Submitted By:</h3>
  <table width="60%">
    <tr>
      <td align="left"><strong>Suarabh Singh</strong></td>
      <td align="right">Roll: cse2023021156</td>
    </tr>
    <tr>
      <td align="left"><strong>Ram Nayak</strong></td>
      <td align="right">Roll: cse2023021152</td>
    </tr>
  </table>
  
  <br /><br />
  <strong>Date:</strong> April 13, 2026
</div>

---

## 1. Abstract
This project explores the implementation of parallel algorithms for matrix multiplication, a core operation in scientific computing and AI. We utilize the **Native Windows Threading API (`windows.h`)** to distribute computational load across multiple processor cores. Additionally, we provide a **Full-Stack Web Dashboard** built with Node.js to visualize performance metrics, including speedup and efficiency, in real-time.

## 2. Introduction
Parallel computing is essential for handling large-scale datasets where sequential processing becomes a bottleneck. Matrix multiplication has a time complexity of $O(N^3)$, making it an ideal candidate for parallelization.

### Why Portable C++ Threads?
While libraries like OpenMP or MPI are popular, working with the **C++11 Standard Threading Library (`std::thread`)** provides deeper insight into:
- Native concurrency management across multiple platforms.
- Dynamic workload distribution and core affinity.
- Standardization and portability (works on Windows, Linux, and Cloud).

---

## 3. Project Architecture
The project follows a decoupled architecture consisting of a high-performance C++ core and a modern web-based monitoring system.

```mermaid
graph LR
    User[User / Browser] -- REST API --> Node[Node.js Server]
    Node -- Exec --> Cpp[C++ Binary]
    Cpp -- Threads --> CPU[Multi-core Processor]
    CPU -- Results --> Cpp
    Cpp -- stdout --> Node
    Node -- JSON --> User
```

### Components:
1.  **Backend (C++):** Handles heavy computation using parallel loops.
2.  **Web Server (Node.js/Express):** Orchestrates the execution and parses metrics.
3.  **Frontend (Vanilla JS/CSS):** Provides an interactive UI for experimenting with matrix sizes.

---

## 4. Methodology & Implementation

### A. Parallel Algorithm (The "Row-Chunk" Approach)
Instead of calculating every element in a separate thread (which creates too much overhead), we divide the rows of Matrix A among the available logical cores.

- **Step 1:** Detect hardware cores using `std::thread::hardware_concurrency()`.
- **Step 2:** Calculate `rows_per_thread = N / num_cores`.
- **Step 3:** Assign each thread a range `[start_row, end_row)`.
- **Step 4:** Each thread performs standard 3-loop multiplication for its assigned rows.
- **Step 5:** Synchronize using `thread.join()`.

### B. High Precision Timing
We use the **C++ Chrono library (`std::chrono`)** to measure execution time. This provides precision in the order of nanoseconds and is platform-independent, which is vital for calculating accurate speedup.

---

## 5. Performance Metrics (Experimental Results)
The following benchmarks were recorded on a 12-core system for a $1000 \times 1000$ matrix.

### Data Table:
| Metric | Sequential | Parallel (12 Threads) |
| :--- | :--- | :--- |
| **Execution Time** | 16.76 s | 2.45 s |
| **Speedup** | 1.0x | **6.85x** |
| **Efficiency** | 100% | 57.1% |

### Observations:
- **Speedup:** The parallel version is nearly 7 times faster than the sequential one.
- **Overhead:** Efficiency is less than 100% due to thread creation overhead and memory bandwidth limitations (Von Neumann bottleneck).

---

## 6. The Web Dashboard
To make the project accessible, we developed a real-time monitoring dashboard.
- **Dynamic Input:** Users can test sizes from 100 up to 2000.
- **Console Feedback:** Displays the raw output from the C++ compiler.
- **Visual Analytics:** Calculates and shows the Speedup factor instantly.

---

## 7. Installation & Usage
1.  **Compile C++ Code:**
    ```bash
    g++ matrix_mul.cpp -o matrix_mul.exe
    ```
2.  **Start Dashboard:**
    ```bash
    npm install
    node server.js
    ```
3.  **Access:** Open `http://localhost:3000` in any web browser.

---

## 8. Conclusion
The project successfully demonstrates the power of parallel computing. By offloading calculations to multiple cores, we achieved a significant reduction in processing time. The integration of a web dashboard further enhances the usability and educational value of the implementation.

---

## 9. Viva Questions & Answers

**Q1: What is the main advantage of Parallel Matrix Multiplication?**
*A: It reduces the wall-clock time required for large computations by utilizing multiple CPU cores simultaneously.*

**Q2: What is "Speedup"?**
*A: Speedup is the ratio of sequential execution time to parallel execution time. $S = T_s / T_p$.*

**Q3: Why isn't efficiency 100%?**
*A: Factors like thread creation overhead, synchronization wait times, and shared memory access contention limit perfect scaling.*

**Q4: What role does the C++11 Threading Library play here?**
*A: It provides the standard API for creating (`std::thread`), managing, and synchronizing (`join`) threads across all major operating systems.*
