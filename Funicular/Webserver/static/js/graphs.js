// graphs.js

//this is untested and not yet completed!!!

// Acceleration, G-Force, and Speed Graph Initialization
let accelerationCtx = document.getElementById('acceleration-graph').getContext('2d');
let gforceCtx = document.getElementById('gforce-graph').getContext('2d');
let speedCtx = document.getElementById('speed-graph').getContext('2d');

let accelerationChart = new Chart(accelerationCtx, {
    type: 'line',
    data: {
        labels: [], // Time or Distance can be used here
        datasets: [{
            label: 'Acceleration (m/s²)',
            data: [],
            borderColor: 'rgba(255, 99, 132, 1)',
            borderWidth: 1,
            fill: false
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});

let gforceChart = new Chart(gforceCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'G-Force (g)',
            data: [],
            borderColor: 'rgba(54, 162, 235, 1)',
            borderWidth: 1,
            fill: false
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});

let speedChart = new Chart(speedCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Speed (km/h)',
            data: [],
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 1,
            fill: false
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});

// Function to update graph data in real-time
function updateGraphs(data) {
    let time = new Date().toLocaleTimeString(); // Example timestamp

    // Update Acceleration Chart
    accelerationChart.data.labels.push(time);
    accelerationChart.data.datasets[0].data.push(data.acceleration);
    accelerationChart.update();

    // Update G-Force Chart
    gforceChart.data.labels.push(time);
    gforceChart.data.datasets[0].data.push(data.gforce);
    gforceChart.update();

    // Update Speed Chart
    speedChart.data.labels.push(time);
    speedChart.data.datasets[0].data.push(data.speed);
    speedChart.update();
}

// Simulate receiving data from the backend (replace with real-time WebSocket data)
setInterval(function() {
    // Example data from sensors
    let sensorData = {
        acceleration: Math.random() * 10,  // Random acceleration value
        gforce: Math.random(),             // Random g-force value
        speed: Math.random() * 100         // Random speed value
    };
    updateGraphs(sensorData);
}, 1000);  // Update every second
