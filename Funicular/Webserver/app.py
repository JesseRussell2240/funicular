from flask import Flask, render_template, Response
from picamera2 import Picamera2
import serial
import time
from io import BytesIO
 
# Flask web server
app = Flask(__name__)
 
# Initialize the camera
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))
picam2.start()
 
# Configure the serial connection for the car controls
SERIAL_PORT = "/dev/serial0"  # Serial port for UART (change if needed)
BAUD_RATE = 9600              # Common baud rate for UART
 
# Initialize the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
 
# Transmit function
def transmit(data):
    try:
 
# Initialize the serial connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        # Send data over UART
        ser.write(data.encode())  # Convert string to bytes
        print("Transmitting:", data)
        #received_data = receive()
        #if received_data:
        #    print("Received:", received_data)
 
    finally:
        ser.close()
 
# Receive function
def receive():
    received_data = ser.read(ser.in_waiting or 1).decode('utf-8')  # Read available data
    return received_data if received_data else None
 
# Movement functions (building and transmitting vector packets)
def move_forward():
    packet = "1"  # Full forward, no turn
    transmit(packet)
 
def move_backward():
    packet = "2"  # Full backward, no turn
    transmit(packet + '\n')
 
def move_stop():
    packet = "0"  
    transmit(packet + '\n')
 
def move_left():
    packet = "4"  # Half power forward, turn left
    transmit(packet + '\n')
    time.sleep(1)
    transmit("5" + '\n')
 
def move_right():
    packet = "3"  # Half power forward, full turn right
    transmit(packet + '\n')
    time.sleep(1)
    transmit("5" + '\n')
 
# Route for live camera feed
def generate_frames():
    while True:
        # Capture a frame from the camera
        frame = picam2.capture_array()
 
        # Convert the frame to a JPEG image
        stream = BytesIO()
        picam2.capture_file(stream, format='jpeg')
        stream.seek(0)
 
        # Yield the frame in a byte stream
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')
 
@app.route('/')
def index():
    return render_template('index.html')
 
# Flask route for video streaming
@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')
 
# Flask routes for movement control
@app.route('/forward')
def forward():
    move_forward()
    return "Forward"
 
@app.route('/backward')
def backward():
    move_backward()
    return "Backward"
 
@app.route('/stop')
def stop():
    move_stop()
    return "Stop"    
 
@app.route('/left')
def left():
    move_left()
    return "Left"
 
@app.route('/right')
def right():
    move_right()
    return "Right"
 
# Close the serial connection when the app shuts down
@app.teardown_appcontext
def close_serial(exception):
    if ser.is_open:
        ser.close()
 
# Start Flask app
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
 
