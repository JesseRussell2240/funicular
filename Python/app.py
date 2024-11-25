from flask import Flask, render_template, jsonify, Response
import threading
import time
import board
import busio
import adafruit_bno08x
from adafruit_bno08x.i2c import BNO08X_I2C
from picamera2 import Picamera2
import cv2
from io import BytesIO
from flask_cors import CORS
from cone_detection import detect_cones


# Flask setup
app = Flask(__name__)
CORS(app)

#add control routes
from controllers.auto_control import control_routes
app.register_blueprint(control_routes, url_prefix='/control')

autonomous_page = False  # Flag for autonomous mode



# Initialize the camera
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))
picam2.start()

# Global variables for sensor data
gyro_data = {'gyro_x': 0.0, 'gyro_y': 0.0, 'gyro_z': 0.0}
acc_data = {'acc_x': 0.0, 'acc_y': 0.0, 'acc_z': 0.0}
vel_data = {'vel_x': 0.0, 'vel_y': 0.0, 'vel_z': 0.0}
last_time = time.time()

# Set up I2C connection for the sensor
i2c = busio.I2C(board.SCL, board.SDA, frequency=400000)  # 400kHz recommended for BNO085
bno = BNO08X_I2C(i2c)

# Enable sensor features
bno.enable_feature(adafruit_bno08x.BNO_REPORT_ACCELEROMETER)
bno.enable_feature(adafruit_bno08x.BNO_REPORT_GYROSCOPE)

# Add a threading lock
data_lock = threading.Lock()

# Function to update sensor data
def update_sensor_data():
    global gyro_data, acc_data, vel_data, last_time
    while True:
        try:
            # Fetch gyroscope data
            gyro_x, gyro_y, gyro_z = bno.gyro
            new_gyro_data = {
                'gyro_x': gyro_y,
                'gyro_y': gyro_z,
                'gyro_z': gyro_x
            }

            # Fetch acceleration data
            acc_x, acc_y, acc_z = bno.acceleration
            new_acc_data = {
                'acc_x': acc_x,
                'acc_y': acc_y,
                'acc_z': acc_z
            }

            # Calculate velocity data
            current_time = time.time()
            dt = current_time - last_time
            last_time = current_time
            new_vel_data = {
                'vel_x': acc_x * dt,
                'vel_y': acc_y * dt,
                'vel_z': acc_z * dt
            }

            # Update global variables with lock
            with data_lock:
                gyro_data = new_gyro_data
                acc_data = new_acc_data
                vel_data = new_vel_data

           # print(gyro_data, acc_data, vel_data)  # For debugging

            time.sleep(0.1)
        except Exception as e:
            print(f"Error in sensor thread: {e}")
            time.sleep(1)

def generate_frames():
    global autonomous_page  # Declare the flag as global
    while True:
        frame = picam2.capture_array()
        
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB
        
        gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

#        Convert grayscale frame to 3 channels (dummy RGB)
        frame = cv2.cvtColor(gray_frame, cv2.COLOR_GRAY2BGR)
        
        if autonomous_page:
            # Call the cone detection function to process the frame
            frame = detect_cones(frame)
        
        #frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB
        stream = BytesIO()
        success, encoded_image = cv2.imencode('.jpeg', frame)
        if success:
            stream.write(encoded_image.tobytes())
            stream.seek(0)

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')



# Flask route to provide sensor data
@app.route('/sensor_data')
def get_sensor_data():
    with data_lock:
        return jsonify(gyro_data)

@app.route('/graph_data')
def get_graph_data():
    with data_lock:
        data = {
            "acc_data": acc_data,
            "vel_data": vel_data
        }
    return jsonify(data)
    
# Route for live camera feed
@app.route('/video_feed')
def video_feed():
    
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Main route for the Flask web app
@app.route('/')
def landing_page():
    autonomous_page = False
    return render_template('landing_page.html')

@app.route('/auto')
def auto_mode():
    global autonomous_page
    autonomous_page = True  
    print(f"Autonomous mode: {autonomous_page}")
    return render_template('auto_mode.html')

@app.route('/manual')
def manual_mode():
    global autonomous_page
    autonomous_page = False
    return render_template('manual_mode.html')

@app.route('/3d')
def model_3d():
    autonomous_page = False
    return render_template('3d_model.html')

@app.route('/video')
def live_video():
    
    return render_template('live_video.html')

@app.route('/graphs')
def graphs():
    autonomous_page = False
    return render_template('graphs.html')
    
# Start the sensor data updating thread
if __name__ == "__main__":
    sensor_thread = threading.Thread(target=update_sensor_data, daemon=True)
    sensor_thread.start()
    app.run(host='0.0.0.0', port=5000, debug=False)
