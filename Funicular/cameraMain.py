import cv2
import time
from Camera.camera_setup import CameraSetup
from Camera.capture_frames import FrameCapture
from Perception.color_detection import ColorDetection
from queue import Queue, Empty
import numpy as np

def main():
    # Initialize camera
    camera = CameraSetup(resolution=(640, 480), framerate=30)

    # Create a queue for frames
    frame_queue = Queue(maxsize=10)

    # Initialize frame capture
    frame_capture = FrameCapture(camera_setup=camera, frame_queue=frame_queue, queue_size=10)
    frame_capture.start()

    # Define the color detection configuration
    config = {
        'blue_lower': (110, 180, 70),  # Tighter HSV range for very blue
        'blue_upper': (130, 255, 255),
        'orange_lower': (5, 180, 70),   # Tighter lower limit for very orange
        'orange_upper': (15, 255, 255)
    }

    # Minimum area for contours to be considered valid
    min_area = 2000  # Further increased threshold for better filtering

    # Initialize perception module
    color_detector = ColorDetection(config=config)

    try:
        while True:
            try:
                frame = frame_queue.get(timeout=1)

                # Process frame for color detection
                detected_cones = color_detector.detect_cones(frame)

                # Draw contours and overlays
                for cone in detected_cones['blue_cones']:
                    cv2.circle(frame, cone, 10, (255, 0, 0), -1)  # Blue cone
                    cv2.putText(frame, 'Blue Cone', (cone[0]-20, cone[1]-10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

                for cone in detected_cones['orange_cones']:
                    cv2.circle(frame, cone, 10, (0, 165, 255), -1)  # Orange cone
                    cv2.putText(frame, 'Orange Cone', (cone[0]-20, cone[1]-10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

                # Draw contours for blue cones
                blue_mask = cv2.inRange(cv2.cvtColor(frame, cv2.COLOR_BGR2HSV), config['blue_lower'], config['blue_upper'])
                blue_mask = cv2.GaussianBlur(blue_mask, (5, 5), 0)  # Reduce noise
                blue_contours, _ = cv2.findContours(blue_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

                # Filter for triangular shapes with area threshold
                for cnt in blue_contours:
                    area = cv2.contourArea(cnt)
                    if area > min_area:  # Check area
                        epsilon = 0.05 * cv2.arcLength(cnt, True)  # Adjusted epsilon for less strict matching
                        approx = cv2.approxPolyDP(cnt, epsilon, True)
                        if len(approx) == 3:  # Check for triangular shape
                            cv2.drawContours(frame, [cnt], -1, (255, 0, 0), 2)  # Draw blue contours
                            M = cv2.moments(cnt)
                            if M['m00'] != 0:
                                cX = int(M['m10'] / M['m00'])
                                cY = int(M['m01'] / M['m00'])
                                cv2.circle(frame, (cX, cY), 10, (255, 0, 0), -1)  # Draw center of triangle

                # Draw contours for orange cones
                orange_mask = cv2.inRange(cv2.cvtColor(frame, cv2.COLOR_BGR2HSV), config['orange_lower'], config['orange_upper'])
                orange_mask = cv2.GaussianBlur(orange_mask, (5, 5), 0)  # Reduce noise
                orange_contours, _ = cv2.findContours(orange_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

                # Filter for triangular shapes with area threshold
                for cnt in orange_contours:
                    area = cv2.contourArea(cnt)
                    if area > min_area:  # Check area
                        epsilon = 0.05 * cv2.arcLength(cnt, True)  # Adjusted epsilon for less strict matching
                        approx = cv2.approxPolyDP(cnt, epsilon, True)
                        if len(approx) == 3:  # Check for triangular shape
                            cv2.drawContours(frame, [cnt], -1, (0, 165, 255), 2)  # Draw orange contours
                            M = cv2.moments(cnt)
                            if M['m00'] != 0:
                                cX = int(M['m10'] / M['m00'])
                                cY = int(M['m01'] / M['m00'])
                                cv2.circle(frame, (cX, cY), 10, (0, 165, 255), -1)  # Draw center of triangle

                # Display frame with detections
                cv2.imshow('Processed Frame', frame)

                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            except Empty:
                print("No frame available.")
                continue
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        frame_capture.stop()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
