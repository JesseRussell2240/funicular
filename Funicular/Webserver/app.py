from flask import Flask, render_template, Response, g
from picamera2 import Picamera2
import serial
import time
from io import BytesIO
import cv2
import numpy as np


# Flag to control autonomous mode
autonomous_mode = True

# Flask web server
app = Flask(__name__)

# Initialize the camera
picam2 = Picamera2()
picam2.configure(picam2.create_video_configuration(main={"size": (640, 480)}))
picam2.start()

# Configure the serial connection for the car controls
SERIAL_PORT = "/dev/ttyAMA10"  # Serial port for UART (change if needed)
BAUD_RATE = 9600                # Common baud rate for UART

# Initialize the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)


# Global variable for steering angle
steering_angle = 30
 

# Transmit function
def transmit(data):
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

        # Send data over UART (encoding to bytes)
        ser.write(data.encode())  # Convert string to bytes
        print("Transmitting:", data)

        # Wait for the response "1\n"
        while True:
            # Read one byte at a time from the serial port
            response = ser.read(1).decode()  # Reading single byte

            if response == '1':
                # Now, wait for the newline character '\n'
                response += ser.read(1).decode()  # Read the newline character '\n'

                if response == '1\n':  # Check if the response is exactly "1\n"
                    print("Received confirmation:", response)
                    break  # Exit loop if the correct response is received

            # Optional: Add a timeout here to break after a certain amount of time
            if ser.timeout:
                print("Timeout: No confirmation received")
                break

    except Exception as e:
        print("Error:", e)

    finally:
        # Close the serial port
        ser.close()


# Movement functions
def move_forward(pwm_percentage=60, steering_angle=30):
    """Move forward with PWM and steering angle control"""
    # Ensure the pwm_percentage is within the range 60-99 and steering_angle is within 0-60
    pwm_percentage = max(60, min(99, pwm_percentage))  # Clamps PWM between 60 and 99
    steering_angle = max(0, min(60, steering_angle))  # Clamps steering angle between 0 and 60

    # Create the movement string
    command = f"+ {pwm_percentage} {steering_angle:02d}"
    transmit(command + '\n')
    print(f"Command: {command}")  # Output the command for debugging

def move_backward(pwm_percentage=60, steering_angle=30):
    """Move backward with PWM and steering angle control"""
    # Ensure the pwm_percentage is within the range 60-99 and steering_angle is within 0-60
    pwm_percentage = max(60, min(99, pwm_percentage))  # Clamps PWM between 60 and 99
    steering_angle = max(0, min(60, steering_angle))  # Clamps steering angle between 0 and 60

    # Create the movement string
    command = f"- {pwm_percentage} {steering_angle:02d}"
    transmit(command + '\n')
    print(f"Command: {command}")  # Output the command for debugging
    autonomous_mode = True
    

def move_left(pwm_percentage=60):
    """Move left with PWM control and default steering angle"""
    # Use default steering angle of 0 (full left)
    steering_angle = 0

    # Ensure the pwm_percentage is within the range 60-99
    pwm_percentage = max(60, min(99, pwm_percentage))  # Clamps PWM between 60 and 99
    pwm_precentage = 0

    # Create the movement string
    command = f"- {pwm_percentage} {steering_angle:02d}"  # Negative for left turn
    transmit(command + '\n')
    print(f"Command: {command}")  # Output the command for debugging

def move_right(pwm_percentage=60):
    """Move right with PWM control and default steering angle"""
    # Use default steering angle of 60 (full right)
    steering_angle = 60

    # Ensure the pwm_percentage is within the range 60-99
    pwm_percentage = max(60, min(99, pwm_percentage))  # Clamps PWM between 60 and 99
    pwm_precentage = 0
    # Create the movement string
    command = f"+ {pwm_percentage} {steering_angle:02d}"  # Positive for right turn
    transmit(command + '\n')
    print(f"Command: {command}")  # Output the command for debugging
    
def move_stop():
    transmit("| 00 30" + '\n')
    print("STOPPED")  # Output the command for debugging

    


def update_steering(midpoint_x, image_center_x):
    global steering_angle, autonomous_mode
    
    # Calculate the offset from the center (how far left or right the midpoint is)
    offset = midpoint_x - image_center_x  # Midpoint is to the left or right of the center
    
    # Normalize the offset to the range [-1, 1] based on the width of the image
    normalized_offset = offset / image_center_x  # This will range from -1 (left) to 1 (right)
    
    # Map the normalized offset to the steering angle range [0, 60]
    new_steering_angle = 30 + normalized_offset * 30  # 30 is the midpoint, with 30 as the max range for each direction
    
    # Clip the steering angle to ensure it's within [0, 60]
    new_steering_angle = max(0, min(60, new_steering_angle))
    
    # Round the steering angle to the nearest integer
    new_steering_angle = round(new_steering_angle)
    
    # Update the global steering angle with a weighted average (smooth the steering)
    steering_angle = steering_angle * 0.6 + new_steering_angle * 0.4  # Weighted average for smooth steering
    
    print(f"Updated Steering Angle: {steering_angle}")
    print(autonomous_mode)
    
    # If in autonomous mode, send the updated steering angle to the MCU
    #if autonomous_mode:
    pwm_percentage = 80  # Set the PWM percentage to 80 for autonomous control
    
    # Create the movement command string
    #command = f"+ {pwm_percentage} {steering_angle:02d}"
    transmit("+ 80 " + str(int(steering_angle)) + '\n')
    #print(f"Command: {command}")  # Output the command for debugging
    time.sleep(.25)
    move_stop()
    time.sleep(0.5)


# Function to detect the largest pylon for both colors
def detect_pylons(frame):
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert frame to RGB

    hsv = cv2.cvtColor(frame, cv2.COLOR_RGB2HSV)

    # Define HSV ranges for the specified colors
    blue_hsv_lower = np.array([100, 150, 50])
    blue_hsv_upper = np.array([130, 255, 255])
    
    red_hsv_lower = np.array([0, 150, 50])
    red_hsv_upper = np.array([10, 255, 255])
    red_hsv_lower2 = np.array([170, 150, 50])
    red_hsv_upper2 = np.array([180, 255, 255])

    # Create masks for the specified colors
    blue_mask = cv2.inRange(hsv, blue_hsv_lower, blue_hsv_upper)
    red_mask = cv2.inRange(hsv, red_hsv_lower, red_hsv_upper)
    red_mask2 = cv2.inRange(hsv, red_hsv_lower2, red_hsv_upper2)

    # Combine masks
    mask = cv2.bitwise_or(blue_mask, red_mask)
    mask = cv2.bitwise_or(mask, red_mask2)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    largest_blue_contour = None
    largest_red_contour = None
    largest_blue_area = 0
    largest_red_area = 0

    for contour in contours:
        area = cv2.contourArea(contour)

        if area > 200:  # Area threshold
            if cv2.countNonZero(blue_mask[contour[:, 0, 1], contour[:, 0, 0]]) > 0:
                if area > largest_blue_area:
                    largest_blue_area = area
                    largest_blue_contour = contour
            elif (cv2.countNonZero(red_mask[contour[:, 0, 1], contour[:, 0, 0]]) > 0 or
                  cv2.countNonZero(red_mask2[contour[:, 0, 1], contour[:, 0, 0]]) > 0):
                if area > largest_red_area:
                    largest_red_area = area
                    largest_red_contour = contour

    # Initialize centroids
    blue_centroid = None
    red_centroid = None

    # Draw the largest blue cone
    if largest_blue_contour is not None:
        cv2.drawContours(frame, [largest_blue_contour], -1, (0, 255, 0), 2)
        M = cv2.moments(largest_blue_contour)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            blue_centroid = (cX, cY)
            cv2.circle(frame, (cX, cY), 7, (255, 0, 0), -1)  # Blue centroid

    # Draw the largest red cone
    if largest_red_contour is not None:
        cv2.drawContours(frame, [largest_red_contour], -1, (0, 255, 0), 2)
        M = cv2.moments(largest_red_contour)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            red_centroid = (cX, cY)
            cv2.circle(frame, (cX, cY), 7, (0, 0, 255), -1)  # Red centroid

    # Draw a line between the blue and red centroids if both are found
    if blue_centroid and red_centroid:
        cv2.line(frame, blue_centroid, red_centroid, (255, 255, 255), 2)  # White line
        midpoint_x = (blue_centroid[0] + red_centroid[0]) // 2
        midpoint_y = (blue_centroid[1] + red_centroid[1]) // 2
        cv2.circle(frame, (midpoint_x, midpoint_y), 7, (0, 255, 255), -1)  # Yellow midpoint dot

        # Determine the center of the image
        image_center_x = frame.shape[1] // 2

        # Update steering angle based on midpoint position
        update_steering(midpoint_x, image_center_x)

        # Draw text indicating if midpoint is left or right
        if midpoint_x < image_center_x:
            position_text = "Left"
        else:
            position_text = "Right"

        # Annotate the frame with position text
        cv2.putText(frame, f"Midpoint: {position_text}", (midpoint_x - 30, midpoint_y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    return frame

# Route for live camera feed
def generate_frames():
    while True:
        frame = picam2.capture_array()
        frame = detect_pylons(frame)

        stream = BytesIO()
        success, encoded_image = cv2.imencode('.jpeg', frame)
        if success:
            stream.write(encoded_image.tobytes())
            stream.seek(0)

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')

@app.route('/start_autonomous')
def start_autonomous():
    autonomous_mode = True
    # Start the camera processing in a separate thread
    return "Autonomous mode starte"

@app.route('/stop_autonomous')
def stop_autonomous():
    autonomous_mode = False
    move_stop()  # Stop the vehicle
    return "Auto_Stop"
  

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/forward')
def forward():
    transmit('\n')
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

@app.teardown_appcontext
def close_serial(exception):
    if ser.is_open:
        ser.close()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
