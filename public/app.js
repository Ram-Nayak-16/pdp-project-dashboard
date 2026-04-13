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
    const perfChartCtx = document.getElementById('perfChart').getContext('2d');
    let perfChart;

    const initChart = () => {
        perfChart = new Chart(perfChartCtx, {
            type: 'bar',
            data: {
                labels: ['Sequential', 'Parallel'],
                datasets: [{
                    label: 'Execution Time (seconds)',
                    data: [0, 0],
                    backgroundColor: [
                        'rgba(0, 210, 255, 0.4)',
                        'rgba(146, 254, 157, 0.4)'
                    ],
                    borderColor: [
                        '#00d2ff',
                        '#92fe9d'
                    ],
                    borderWidth: 2,
                    borderRadius: 8,
                    barPercentage: 0.6
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        display: false
                    },
                    tooltip: {
                        backgroundColor: 'rgba(15, 12, 41, 0.9)',
                        titleFont: { family: 'Outfit', size: 14 },
                        bodyFont: { family: 'Outfit', size: 13 },
                        padding: 12,
                        borderColor: 'rgba(255, 255, 255, 0.1)',
                        borderWidth: 1
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        grid: {
                            color: 'rgba(255, 255, 255, 0.05)',
                            drawBorder: false
                        },
                        ticks: {
                            color: '#b0b0b0',
                            font: { family: 'Outfit' }
                        }
                    },
                    x: {
                        grid: {
                            display: false
                        },
                        ticks: {
                            color: '#ffffff',
                            font: { family: 'Outfit', weight: '600' }
                        }
                    }
                },
                animation: {
                    duration: 1500,
                    easing: 'easeOutQuart'
                }
            }
        });
    };

    const updateChart = (seqTime, parTime) => {
        if (!perfChart) return;
        perfChart.data.datasets[0].data = [seqTime, parTime];
        perfChart.update();
    };

    initChart();

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

            // Update Graph
            updateChart(data.sequentialTime, data.parallelTime);

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
