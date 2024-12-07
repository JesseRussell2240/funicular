from flask import Flask, render_template, jsonify, Response, request
import threading
import time
import json
import board
import busio
import adafruit_bno08x
from adafruit_bno08x.i2c import BNO08X_I2C
from picamera2 import Picamera2
import cv2
from io import BytesIO
from flask_cors import CORS
from cone_detection import detect_cones
from controllers.auto_control import control_routes
import serial
import threading

auto_flag = False
auto_lock = threading.Lock()

# Flask setup
app = Flask(__name__)
CORS(app)
app.register_blueprint(control_routes, url_prefix='/control')  # Register with a prefix (optional)

# Initialize the camera
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))

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

# Serial connection configuration
SERIAL_PORT = "/dev/serial0"  # Serial port for UART (change if needed)
BAUD_RATE = 9600              # Common baud rate for UART



# Initialize the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

stmIsHome = False            # Homing flag
steering_center = 30          # Steering center position
steering_max = 60             # Maximum turning value LEFT
steering_min = 0             # Minimum turning value 
half_steering_left = 15      # Half turn left position
half_steering_right = 45     # Half turn right position
full_power = 60              # Full forward/backward power
half_power = 40              # Half power


def transmit(data):
    global stmIsHome, steering_center, steering_max

    try:
        # Ensure the serial port is open
        if not ser.is_open:
            ser.open()

        # Check if the system is homed
        if not stmIsHome:
            # Send the homing command
            homing_packet = "* 00 30\n"
            ser.write(homing_packet.encode())  # Convert string to bytes
            print(f"Transmitting homing packet: {homing_packet}")
            time.sleep(1)
            # Receive the maximum turning value
            received_data = receive()
            print(f"GOT {received_data}")
            if received_data:
                try:
                    # Parse the received maximum turning value
                    steering_max = int(received_data.strip())
                    steering_center = steering_max // 2  # Calculate the steering center
                    
                    steeringIncrement = steering_center // 2
                    half_steering_left = steering_center - steeringIncrement
                    half_steering_right = steering_center + steeringIncrement
                    
                    stmIsHome = True  # Update the homing flag
                    print(f"Received max turning: {steering_max}, Steering center: {steering_center}")
                    
                except ValueError:
                    print("Error parsing received max turning value.")
                    return
            else:
                print("No response received for homing packet.")
                return

        # Transmit the provided data over UART
        ser.write(data.encode())  # Convert string to bytes
        print(f"Transmitting: {data}")

        # Receive response after transmission (you can modify this as per your protocol)
          # Function to read data from serial port

        while True:
            # Simulate receiving data (replace this with actual data retrieval logic)
            #received_data = input("Enter received data for testing: ")  # Example for testing
            received_data = receive()
            # Check if the received data contains "1"
            if received_data and "1" in received_data:  # Replace 'expected_response' with your actual check
                print("Handshake confirmed")
                break  # Exit the loop once the condition is met
            else:
                print("Error in transmission, waiting...")
                
    except serial.SerialException as e:
        print(f"Error with serial connection: {e}")
    except Exception as e:
        print(f"General error: {e}")
    finally:
        # Optionally, close the serial port after transmission
        ser.close()

def receive():
    try:
        # Read available data from the serial buffer
        received_data = ser.read(ser.in_waiting or 1).decode('utf-8')  # Read data
        return received_data if received_data else None
    except serial.SerialException as e:
        print(f"Error reading data: {e}")
        return None

# Movement control functions
def move_forward():
    steering = steering_center  # No turn
    packet = f"+ {full_power:02} {steering_center}"
    transmit(packet + '\n')

def move_backward():
    steering = steering_center  # No turn
    packet = f"- {full_power:02} {steering_center}"
    transmit(packet + '\n')

def move_stop():
    speed = 0  # No movement
    packet = f"| {speed:02} {steering_center}"
    transmit(packet + '\n')
    
def move_left():
    packet = f"+ {half_power:02} {steering_min + 7}"
    transmit(packet + '\n')
    #time.sleep(1)
    #move_stop()

def move_right():
    packet = f"+ {half_power:02} {steering_max - 7}"
    transmit(packet + '\n')
    #time.sleep(1)
    #move_stop()

def move_left_half():
    packet = f"+ {half_power:02} {half_steering_left}"
    transmit(packet + '\n')
    #time.sleep(1)
    #move_stop()

def move_right_half():
    packet = f"+ {half_power:02} {half_steering_right}"
    transmit(packet + '\n')
    #ime.sleep(1)
    #move_stop()
    
auto_flag = False
    
def drive_to_target(center_x, center_y, cone_size):
    """Move the robot to the center of the two cones (target coordinates)."""
    print(f"Driving to coordinates: {center_x}, {center_y} with cone size: {cone_size}")

    # Calculate the proportional error in the x-direction
    target_center_x = 320  # Assuming the center of the camera frame is at 320 (horizontal center)
    error_x = center_x - target_center_x

    # Set the proportional gain for left/right movement
    Kp_x = 0.10  # You may need to tune this value to adjust responsiveness
    turn_speed = Kp_x * error_x  # This determines how fast to turn (proportional to the error)

    # Set the proportional gain for speed (based on the size of the cone)
    Kp_size = 0.5  # You may need to tune this value for better control
    #speed = Kp_size * cone_size  # This adjusts speed based on the cone size
    #speed = 65
    speed = Kp_size * full_power
    
    # Limit the speed for safety
    max_speed = 99
    if speed > max_speed:
        speed = max_speed
    #print(f"speed: {speed}")
    print(f"turn factor: {turn_speed}")

        
     # Determine direction based on error_x
    if error_x < -15:  # If the error is negative, it means the target is to the left
        turn_value = max(steering_min, min(steering_max, int(steering_center + turn_speed)))  # Clamp between 5 and 55
        packet = f"+ {int(speed)} {turn_value}"  # Turn left with proportional speed
        transmit(packet + '\n')
    elif error_x > 15:  # If the error is positive, it means the target is to the right
        turn_value = max(steering_min, min(steering_max, int(steering_center + turn_speed)))
        packet = f"+ {int(speed)} {turn_value}"  # Turn right with proportional speed
        transmit(packet + '\n')
    else:
        # If the target is centered, just move forward
        packet = f"+ {int(speed)} 30"  # Move forward
        transmit(packet + '\n')



auto_flag = False
auto_lock = threading.Lock()
     
def autonomous_loop():
    """Function to run autonomous mode in a separate thread."""
    global auto_flag
    json_file_path = '/home/rpi/Desktop/Beta/Python/hailo-rpi5-examples/cone_detection.json'

    while True:
        with auto_lock:
            if not auto_flag:
                break

        try:
            # Open and read the JSON file
            with open(json_file_path, 'r') as json_file:
                data = json.load(json_file)

            # Extract cone data
            red_cone = data.get("largest_red_cone", {"center": [0, 0], "area": 0})
            blue_cone = data.get("largest_blue_cone", {"center": [0, 0], "area": 0})

            # Extract properties
            red_x, red_y = red_cone['center']
            blue_x, blue_y = blue_cone['center']
            red_area = red_cone.get('area', 0)
            blue_area = blue_cone.get('area', 0)

            if red_area > 0 and blue_area > 0:
                # Case: Both cones are non-zero
                center_x = (red_x + blue_x) / 2
                center_y = (red_y + blue_y) / 2
                avg_area = (red_area + blue_area) / 2
                print("Both cones detected with non-zero area. Driving to target.")
                drive_to_target(center_x, center_y, avg_area)

            elif red_area > 0 or blue_area > 0:
                # Case: One cone is non-zero
                if red_area > 0:
                    print("Single red cone detected.")
                    cone_x = red_x
                else:
                    print("Single blue cone detected.")
                    cone_x = blue_x

                screen_midpoint = 320  # Adjust based on your screen resolution
                if cone_x < screen_midpoint:
                    if red_area > 0:
                        print("Red cone on the left. Turning right half.")
                        move_right_half()
                    else:
                        print("Blue cone on the left. Turning left full.")
                        move_left()
                else:
                    if red_area > 0:
                        print("Red cone on the right. Turning right full.")
                        move_right()
                    else:
                        print("Blue cone on the right. Turning left half.")
                        move_left_half()
            else:
                # Case: Both cones have zero values
                print("No valid cones detected. Stopping the car.")
                move_stop()

        except FileNotFoundError:
            print(f"JSON file not found: {json_file_path}")
        except json.JSONDecodeError:
            print("Error decoding JSON file. Ensure it is correctly formatted.")
        except KeyError as e:
            print(f"Unexpected key error: {e}")

        time.sleep(0.1)    
   
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

            time.sleep(0.1)
        except Exception as e:
            print(f"Error in sensor thread: {e}")
            time.sleep(1)

# Function to generate frames for the video feed
def generate_frames():
    global autonomous_page  # Declare the flag as global
    while True:
        try:
            frame = picam2.capture_array()
            
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB
            
            # Convert the frame to JPEG
            stream = BytesIO()
            success, encoded_image = cv2.imencode('.jpeg', frame)
            if success:
                stream.write(encoded_image.tobytes())
                stream.seek(0)

            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')
        except Exception as e:
            print(f"Error in generating frames: {e}")
            time.sleep(1)

@app.route('/control/move', methods=['POST'])
def move():
    global movement_status
    data = request.json
    direction = data.get('direction', 'stop')
    movement_status = direction

    # Print button press to the console
    print(f"[MOVE] Button pressed: {direction.capitalize()}")

    # Perform the movement action
    if direction == "forward":
        move_forward()
    elif direction == "backward":
        move_backward()
    elif direction == "left":
        move_left()
    elif direction == "right":
        move_right()
    else:
        move_stop()

    return jsonify({"status": f"Moving {direction}"}), 200

@app.route('/control/bluetooth', methods=['POST'])
def bluetooth():
    global bluetooth_enabled
    action = request.json.get('action', 'disable')
    bluetooth_enabled = (action == 'enable')
    status = "enabled" if bluetooth_enabled else "disabled"
 
    # Print button press to the console
    action_text = "Enable Bluetooth" if bluetooth_enabled else "Disable Bluetooth"
    print(f"[BLUETOOTH] Button pressed: {action_text}")
 
    if bluetooth_enabled:
        try:
            import pygame
            pygame.init()
            pygame.joystick.init()
 
            if pygame.joystick.get_count() == 0:
                print("[BLUETOOTH] No controller detected.")
                return jsonify({"status": "error", "message": "No controller detected"}), 400
 
            controller = pygame.joystick.Joystick(0)
            controller.init()
 
            print("[BLUETOOTH] PS4 controller connected.")
 
            running = True
            while running and bluetooth_enabled:
                pygame.event.pump()
 
                # Left joystick for steering
                left_x = controller.get_axis(0)

                

                # Determine direction and movement based on joystick and triggers
                direction = "|"  # Default direction (Neutral)
                pwm_percentage = 0
                steering_angle = steering_center
                
                # Determine movement direction based on the joystick
                if left_x <= -0.4:
                    steering_angle = int(30 + (left_x * 30))
                    steering_angle = max(7, min(53, steering_angle))  # Clamp between 7 and 53
                elif left_x >= 0.4:
                    steering_angle = int(30 + (left_x * 30))
                    steering_angle = max(10, min(50, steering_angle))  # Clamp between 7 and 53

                # Handle triggers for forward/backward movement
                left_trigger = controller.get_axis(2)
                right_trigger = controller.get_axis(5)

                if left_trigger > 0:  # Forward movement
                    pwm_percentage = int(60 + (left_trigger * 30))  # Forward speed calculation
                    direction = "-"  # Forward direction
                elif right_trigger > 0:  # Backward movement
                    pwm_percentage = int(60 + (right_trigger * 30))  # Backward speed calculation
                    direction = "+"  # Backward direction
                else:  # No trigger pressed
                    pwm_percentage = 0  # No movement
                    direction = "|"  # Stopped

                # Construct the transmit packet
                packet = f"{direction} {pwm_percentage} {steering_angle}"
                transmit(packet + '\n')
                # Transmit the constructed packet
                #print(f"[BLUETOOTH] Transmit Packet: {transmit_packet}")
 
        except Exception as e:
            print(f"[BLUETOOTH] Error: {str(e)}")
            return jsonify({"status": "error", "message": str(e)}), 500
 
    return jsonify({"status": f"Bluetooth {status}"}), 200



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
    try:
        picam2.start()  # Start the camera when the video feed route is called
    except Exception as e:
        print(f"Camera error while starting: {e}")
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Main route for the Flask web app
@app.route('/')
def landing_page():
    try:
        picam2.stop()  # Stop the camera when landing page is called
    except Exception as e:
        print(f"Camera error while stopping: {e}")
    return render_template('landing_page.html')

@app.route('/auto')
def auto_mode():
    global autonomous_page
    autonomous_page = True
    try:
        picam2.stop()  # Stop the camera when entering auto mode
    except Exception as e:
        print(f"Camera error while stopping: {e}")
    return render_template('auto_mode.html')

@app.route('/manual')
def manual_mode():
    global autonomous_page
    autonomous_page = False
    try:
        picam2.stop()  # Stop the camera when entering manual mode
    except Exception as e:
        print(f"Camera error while stopping: {e}")
    return render_template('manual_mode.html')

@app.route('/3d')
def model_3d():
    try:
        picam2.stop()  # Stop the camera when 3D model page is called
    except Exception as e:
        print(f"Camera error while stopping: {e}")
    return render_template('3d_model.html')

@app.route('/video')
def live_video():
    return render_template('live_video.html')

@app.route('/graphs')
def graphs():
    try:
        picam2.stop()  # Stop the camera when graph page is called
    except Exception as e:
        print(f"Camera error while stopping: {e}")
    return render_template('graphs.html')

@app.route('/control/start_autonomous', methods=['POST'])
def start_autonomous():
    global auto_flag
    with auto_lock:
        if auto_flag:
            return jsonify({"message": "Autonomous mode is already running"}), 400
        auto_flag = True
    
    threading.Thread(target=autonomous_loop, daemon=True).start()
    print("Start Autonomous Mode triggered!")
    return jsonify({"message": "Autonomous mode started"}), 200

@app.route('/control/stop_autonomous', methods=['POST'])
def stop_autonomous():
    global auto_flag
    with auto_lock:
        auto_flag = False
    print("Stop Autonomous Mode triggered!")
    move_stop()  # Stop the robot
    return jsonify({"message": "Autonomous mode stopped"}), 200

# Start the sensor data updating thread
if __name__ == "__main__":
    sensor_thread = threading.Thread(target=update_sensor_data, daemon=True)
    sensor_thread.start()
    app.run(host='0.0.0.0', port=5000, debug=False)
