import cv2
import numpy as np
import time
import cv2
import sys

#from ultralytics import YOLO

# Load your trained YOLO model
#model = YOLO("best.pt")

# Initialize global variables
steering_angle = 30  # Start with a neutral steering angle




def detect_cones(frame):
    """
    Process the frame and invoke the detection pipeline.
    This function will start the detection from detection.py.
    """
    # Convert the frame to the required format (if necessary)
    # Depending on how detection.py expects the frame, you may need to adjust the frame format
    # For example, if GStreamerDetectionApp expects the frame in a specific format
    # or needs to be passed via a buffer, you can handle that here.

    # For now, let's simulate sending the frame into the pipeline. If detection.py handles GStreamer
    # or other complex workflows, ensure it works with the callback properly.
    
    sys.argv = [
        "script.py",  # Name of the script (you can leave it as "script.py" or as whatever the script name is)
        "--hef-path", "/home/rpi/Desktop/Beta/Python/hailo-rpi5-examples/resources/funicular2.hef",
        "--labels-json", "/home/rpi/Desktop/hailo-rpi5-examples/resources/cones.json",
        "--use-frame"  # Adding the --use-frame argument
    ]

    # Initialize the callback class
    user_data = user_app_callback_class()

    # Initialize the detection pipeline from detection.py
    app = GStreamerDetectionApp(app_callback, user_data)
    
    # Run the pipeline to detect cones (this is assuming app.run() handles streaming and callbacks)
    app.run()  # Running the detection pipeline

    # For illustration, let's just return the frame (you could process it further here)
    return frame







def update_steering(midpoint_x, image_center_x):
    global steering_angle

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
    
    # If in autonomous mode, send the updated steering angle to the MCU
    pwm_percentage = 80  # Set the PWM percentage to 80 for autonomous control
    transmit("+ 80 " + str(int(steering_angle)) + '\n')
    time.sleep(0.25)
    move_stop()
    time.sleep(0.5)
