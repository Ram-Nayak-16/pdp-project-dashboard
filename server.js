const express = require('express');
const { exec } = require('child_process');
const path = require('path');
const os = require('os');
const app = express();
const port = process.env.PORT || 3000;

app.use(express.static('public'));
app.use(express.json());

// Detect the compiled binary based on OS
const isWindows = process.platform === 'win32';
const binaryName = isWindows ? 'matrix_mul.exe' : './matrix_mul';

app.get('/run', (req, res) => {
    const size = req.query.size || 500;
    
    // Execute the portable C++ binary
    exec(`${binaryName} ${size}`, { cwd: __dirname }, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).json({ error: error.message, stderr });
        }
        
        // Parse results from stdout
        // Standard patterns from matrix_mul.cpp output:
        // Sequential Time: 0.9821 seconds
        // Parallel Time:   0.2645 seconds
        // Speedup:      3.71x
        // Efficiency:   92.7%
        // Cores Used:   4
        
        const results = {
            sequentialTime: parseFloat(stdout.match(/Sequential Time:\s+([\d.]+)/)?.[1]),
            parallelTime: parseFloat(stdout.match(/Parallel Time:\s+([\d.]+)/)?.[1]),
            speedup: parseFloat(stdout.match(/Speedup:\s+([\d.]+)/)?.[1]),
            efficiency: parseFloat(stdout.match(/Efficiency:\s+([\d.]+)/)?.[1]),
            cores: parseInt(stdout.match(/Cores Used:\s+(\d+)/)?.[1]),
            verified: stdout.includes("SUCCESS"),
            raw: stdout
        };
        
        // Debug log for cloud environments
        console.log(`[${new Date().toISOString()}] Ran ${size}x${size} matrix. Speedup: ${results.speedup}x`);
        
        res.json(results);
    });
});

app.listen(port, () => {
    console.log(`=================================================`);
    console.log(` Dashboard server running at port: ${port}`);
    console.log(` Mode: ${isWindows ? 'Windows' : 'Linux/Unix'}`);
    console.log(` Binary: ${binaryName}`);
    console.log(`=================================================`);
});
