# 🚀 Parallel Matrix Multiplier Dashboard

[![Live Demo](https://img.shields.io/badge/Live-Demo-brightgreen?style=for-the-badge&logo=render)](https://pdp-project-dashboard.onrender.com/)
[![Built with C++](https://img.shields.io/badge/Core-C%2B%2B11-blue?style=for-the-badge&logo=c%2B%2B)](https://pdp-project-dashboard.onrender.com/)
[![Web Console](https://img.shields.io/badge/UI-Express.js-black?style=for-the-badge&logo=node.js)](https://pdp-project-dashboard.onrender.com/)

A high-performance Computing (HPC) project demonstrating **Parallel Matrix Multiplication** using a custom C++ engine and a real-time web dashboard.

---

## 🌐 Live URL
**Check it out here: [https://pdp-project-dashboard.onrender.com/](https://pdp-project-dashboard.onrender.com/)**

---

## ✨ Features
- **Hybrid Parallel Core**: Uses Native Windows Threads locally and **OpenMP** on the cloud for maximum performance.
- **Real-time Analytics**: Visualizes Sequential vs. Parallel execution times, Speedup Factors, and Efficiency.
- **Interactive Performance Charts**: Built-in Chart.js integration for visual comparison of execution metrics.
- **Glassmorphism UI**: A premium, modern dashboard built with Vanilla CSS and JS.
- **System Detection**: Automatically detects hardware cores to optimize thread distribution.

## 🛠️ Tech Stack
- **Backend Core**: C++11 (Threading, Chrono, OpenMP)
- **Web Server**: Node.js & Express.js
- **Frontend**: HTML5, CSS3 (Glassmorphism), Vanilla JavaScript
- **Infrastructure**: Git, GitHub, Render (Cloud Hosting)

## 📊 Performance Benchmark (Example)
On a standard 8-core cloud environment (Render), a $600 \times 600$ matrix multiplication yields:
- **Sequential Time**: ~1.5s
- **Parallel Time**: ~0.2s
- **Speedup**: **~7.5x**
- **Verification**: ✅ SUCCESS

## 🚀 How to Run Locally
1. **Clone the repository**:
   ```bash
   git clone https://github.com/Ram-Nayak-16/pdp-project-dashboard.git
   cd pdp-project-dashboard
   ```
2. **Compile the C++ Core**:
   ```bash
   npm run build:win
   ```
3. **Start the Dashboard**:
   ```bash
   npm start
   ```
4. **Access**: Open `http://localhost:3000`

---

## 👨‍💻 Developed By
- **Ram Nayak**
- **Suarabh Singh**

*Part of the Parallel Computation and Distributed System (PDP) Course.*
