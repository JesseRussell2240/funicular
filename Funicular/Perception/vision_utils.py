# vision_utils.py

import cv2
import logging

class VisionUtils:
    def __init__(self):
        """
        Initialize the VisionUtils class with logging.
        """
        # Setup logger
        self.logger = logging.getLogger('VisionUtils')
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)
        
    def draw_cones(self, frame, cones, color, label):
        """
        Draw circles and labels around detected cones.
        
        Args:
            frame (numpy.ndarray): The image frame in BGR format.
            cones (list): List of (x, y) tuples representing cone centers.
            color (tuple): BGR color tuple for drawing.
            label (str): Label for the cone type.
        
        Returns:
            numpy.ndarray: The image frame with drawings.
        """
        for (x, y) in cones:
            cv2.circle(frame, (x, y), 10, color, -1)
            cv2.putText(frame, label, (x + 15, y + 5), cv2.FONT_HERSHEY_SIMPLEX, 
                        0.5, color, 2)
            self.logger.debug(f"Drew {label} cone at ({x}, {y}).")
        return frame
    
    def display_fps(self, frame, fps):
        """
        Display the current FPS on the frame.
        
        Args:
            frame (numpy.ndarray): The image frame in BGR format.
            fps (float): Frames per second value.
        
        Returns:
            numpy.ndarray: The image frame with FPS displayed.
        """
        cv2.putText(frame, f"FPS: {fps:.2f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 
                    1, (0, 255, 0), 2)
        self.logger.debug(f"Displayed FPS: {fps:.2f}.")
        return frame
