document.addEventListener('DOMContentLoaded', () => {
    const runBtn = document.getElementById('runBtn');
    const matrixSizeInput = document.getElementById('matrixSize');
    const seqTimeEl = document.getElementById('seqTime');
    const parTimeEl = document.getElementById('parTime');
    const speedupEl = document.getElementById('speedup');
    const efficiencyEl = document.getElementById('efficiency');
    const coreCountEl = document.getElementById('coreCount');
    const verifyStatusEl = document.getElementById('verifyStatus');
    const rawOutputEl = document.getElementById('rawOutput');
    const btnLoader = document.getElementById('btnLoader');

    const runBenchmark = async () => {
        const size = matrixSizeInput.value;
        if (!size || size < 100) {
            alert('Please enter a matrix size of at least 100.');
            return;
        }

        // UI Loading State
        runBtn.classList.add('loading');
        runBtn.disabled = true;
        rawOutputEl.textContent = 'Executing C++ process... please wait.';

        try {
            const response = await fetch(`/run?size=${size}`);
            const data = await response.json();

            if (data.error) {
                rawOutputEl.textContent = `Error: ${data.error}\n${data.stderr}`;
                return;
            }

            // Update Metrics with animations
            animateValue(seqTimeEl, data.sequentialTime, 's');
            animateValue(parTimeEl, data.parallelTime, 's');
            animateValue(speedupEl, data.speedup, 'x');
            animateValue(efficiencyEl, data.efficiency, '%');

            coreCountEl.textContent = data.cores;
            verifyStatusEl.textContent = data.verified ? '✅ SUCCESS' : '❌ FAILED';
            verifyStatusEl.style.color = data.verified ? '#92fe9d' : '#ff4b2b';
            
            rawOutputEl.textContent = data.raw;

        } catch (error) {
            console.error('Fetch error:', error);
            rawOutputEl.textContent = 'Failed to communicate with server.';
        } finally {
            runBtn.classList.remove('loading');
            runBtn.disabled = false;
        }
    };

    const animateValue = (element, value, suffix = '') => {
        if (!value && value !== 0) {
            element.textContent = '-';
            return;
        }
        
        const start = 0;
        const end = value;
        const duration = 1000;
        let startTime = null;

        const step = (timestamp) => {
            if (!startTime) startTime = timestamp;
            const progress = Math.min((timestamp - startTime) / duration, 1);
            const current = (progress * (end - start) + start).toFixed(progress === 1 ? 4 : 2);
            element.textContent = current + suffix;
            if (progress < 1) {
                window.requestAnimationFrame(step);
            }
        };

        window.requestAnimationFrame(step);
    };

    runBtn.addEventListener('click', runBenchmark);
});
