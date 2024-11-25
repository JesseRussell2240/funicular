import cv2
import numpy as np
import time
from ultralytics import YOLO

# Load your trained YOLO model
model = YOLO("best.pt")

# Initialize global variables
steering_angle = 30  # Start with a neutral steering angle

def detect_cones(frame):
    original_height, original_width = frame.shape[:2]  # Get original frame dimensions

    # Resize frame to match YOLO input size (640x640)
    frame_resized = cv2.resize(frame, (640, 640))  # Resize frame for YOLO input
    frame_rgb = cv2.cvtColor(frame_resized, cv2.COLOR_BGR2RGB)  # Convert to RGB for YOLO
    results = model(frame_rgb)  # Perform detection on the RGB image
    
    for result in results:
        if hasattr(result, 'boxes'):  # Ensure the result has boxes
            for box in result.boxes:
                x1, y1, x2, y2 = map(int, box.xyxy[0])  # Get bounding box coordinates

                # Rescale bounding box coordinates back to the original frame size
                x1 = int(x1 * original_width / 640)
                y1 = int(y1 * original_height / 640)
                x2 = int(x2 * original_width / 640)
                y2 = int(y2 * original_height / 640)

                confidence = float(box.conf)  # Get confidence score
                class_index = int(box.cls)  # Get class index
                label = model.names[class_index]  # Get label name

                # Draw bounding box on the original frame
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(frame, f"{label} {confidence:.2f}", (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
        else:
            print("No bounding boxes found in the result")

    return frame  # Return the processed frame








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
