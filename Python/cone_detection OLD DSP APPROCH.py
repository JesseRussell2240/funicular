import cv2
import numpy as np

def detect_cones(frame):
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
    
    # If in autonomous mode, send the updated steering angle to the MCU
    pwm_percentage = 80  # Set the PWM percentage to 80 for autonomous control
    transmit("+ 80 " + str(int(steering_angle)) + '\n')
    time.sleep(0.25)
    move_stop()
    time.sleep(0.5)
