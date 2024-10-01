from flask import Flask, render_template, Response, request
import cv2
import socket

app = Flask(__name__)

# Camera setup (use OpenCV for capturing video)
camera = cv2.VideoCapture(0)  # Change index if necessary for your camera

# Socket configuration for sending control commands
CONTROL_SERVER_HOST = 'localhost'  # Adjust for the control server IP
CONTROL_SERVER_PORT = 5001  # Port for sending control commands

def send_control_command(command):
    """Send a control command to the device via socket."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((CONTROL_SERVER_HOST, CONTROL_SERVER_PORT))
        sock.sendall(command.encode())

def generate_frames():
    """Generator function to yield camera frames as a video stream."""
    while True:
        success, frame = camera.read()  # Capture frame-by-frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    """Render the main page with control buttons and video feed."""
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    """Route to serve video feed."""
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/control', methods=['POST'])
def control():
    """Handle movement control commands from the web interface."""
    command = request.form.get('command')
    if command:
        send_control_command(command)
    return '', 204

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
