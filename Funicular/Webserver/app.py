# app.py
from flask import Flask, render_template, Response, request
from flask_socketio import SocketIO
import camera_stream
import ros_listener

app = Flask(__name__)
socketio = SocketIO(app)

# Initialize ROS listener with SocketIO
ros_listener.socketio = socketio

@app.route('/')
def index():
    """Render the main dashboard page."""
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    """Route to display the live camera feed."""
    return Response(camera_stream.gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@socketio.on('connect')
def handle_connect():
    print('Client connected')

@socketio.on('disconnect')
def handle_disconnect():
    print('Client disconnected')

@app.route('/control', methods=['POST'])
def control():
    """Handle control commands for the car (manual mode)."""
    data = request.json
    direction = data.get('direction')
    print(f"Received control command: {direction}")
    # Here, you can interface with ROS to send control commands for manual driving
    # rospy.Publisher('/car/manual_control', String, queue_size=10).publish(direction)
    return 'OK', 200

if __name__ == '__main__':
    # Start the ROS listener in a separate thread
    import threading
    ros_thread = threading.Thread(target=ros_listener.ros_init)
    ros_thread.start()

    # Start Flask server with SocketIO
    socketio.run(app, host='0.0.0.0', port=5000)
