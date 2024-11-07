// controls.js

// WebSocket connection for real-time communication (optional)
let socket = new WebSocket("ws://localhost:5000/control");

document.addEventListener('keydown', function(event) {
    switch (event.key) {
        case 'w':
            sendControl('forward');
            break;
        case 'a':
            sendControl('left');
            break;
        case 's':
            sendControl('backward');
            break;
        case 'd':
            sendControl('right');
            break;
    }
});

// Handle gamepad inputs
window.addEventListener("gamepadconnected", function(e) {
    console.log("Gamepad connected at index " + e.gamepad.index);
    requestAnimationFrame(updateGamepad);
});

function updateGamepad() {
    let gamepads = navigator.getGamepads();
    if (gamepads[0]) {
        let gp = gamepads[0];
        // Example: Left joystick controls for car movement
        if (gp.axes[0] < -0.5) {
            sendControl('left');
        } else if (gp.axes[0] > 0.5) {
            sendControl('right');
        }
        if (gp.axes[1] < -0.5) {
            sendControl('forward');
        } else if (gp.axes[1] > 0.5) {
            sendControl('backward');
        }
    }
    requestAnimationFrame(updateGamepad);
}

// Function to send control commands to the server
function sendControl(direction) {
    console.log(`Sending ${direction} command`);
    // Send control command via WebSocket or HTTP request
    if (socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({ direction: direction }));
    } else {
        // Fallback to HTTP request
        fetch('/control', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ direction: direction })
        }).then(response => console.log(response));
    }
}

// Start Autonomous mode
function startAutonomousMode() {
    fetch('/start_autonomous')  // Send a GET request to the correct Flask route
        .then(response => response.text())
        .then(data => {
            console.log(data);
            // You can update the button visibility or other UI elements based on the response
        })
        .catch(error => console.error('Error:', error));
}

// Stop Autonomous mode
function stopAutonomousMode() {
    fetch('/stop_autonomous')  // Send a GET request to the correct Flask route
        .then(response => response.text())
        .then(data => {
            console.log(data);
            // You can update the button visibility or other UI elements based on the response
        })
        .catch(error => console.error('Error:', error));
}

