# color_detection.py

import cv2
import numpy as np
import logging

class ColorDetection:
    def __init__(self, config):
        """
        Initialize the ColorDetection class with color ranges and logging.
        
        Args:
            config (dict): Configuration dictionary containing color ranges.
        """
        self.blue_lower = config['blue_lower']
        self.blue_upper = config['blue_upper']
        self.orange_lower = config['orange_lower']
        self.orange_upper = config['orange_upper']
        
        # Setup logger
        self.logger = logging.getLogger('ColorDetection')
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)
        
    def detect_cones(self, frame):
        """
        Detect blue and orange cones in the given frame.
        
        Args:
            frame (numpy.ndarray): The image frame in BGR format.
        
        Returns:
            dict: Dictionary containing lists of detected blue and orange cone positions.
        """
        self.logger.debug("Starting cone detection.")
        processed_frame = frame.copy()
        
        # Convert BGR to HSV
        hsv = cv2.cvtColor(processed_frame, cv2.COLOR_BGR2HSV)
        
        # Detect blue cones
        blue_mask = cv2.inRange(hsv, self.blue_lower, self.blue_upper)
        blue_contours = self._get_contours(blue_mask)
        blue_cones = self._get_cone_centers(blue_contours)
        self.logger.debug(f"Detected {len(blue_cones)} blue cones.")
        
        # Detect orange cones
        orange_mask = cv2.inRange(hsv, self.orange_lower, self.orange_upper)
        orange_contours = self._get_contours(orange_mask)
        orange_cones = self._get_cone_centers(orange_contours)
        self.logger.debug(f"Detected {len(orange_cones)} orange cones.")
        
        return {
            'blue_cones': blue_cones,
            'orange_cones': orange_cones
        }
    
    def _get_contours(self, mask):
        """
        Find contours in the given mask.
        
        Args:
            mask (numpy.ndarray): Binary mask image.
        
        Returns:
            list: List of contours found in the mask.
        """
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        self.logger.debug(f"Found {len(contours)} contours.")
        return contours
    
    def _get_cone_centers(self, contours, min_area=500):
        """
        Calculate the center points of contours that are likely to be cones.
        
        Args:
            contours (list): List of contours.
            min_area (int): Minimum area threshold to filter out noise.
        
        Returns:
            list: List of (x, y) tuples representing cone centers.
        """
        centers = []
        for cnt in contours:
            area = cv2.contourArea(cnt)
            if area > min_area:
                M = cv2.moments(cnt)
                if M['m00'] != 0:
                    cX = int(M['m10'] / M['m00'])
                    cY = int(M['m01'] / M['m00'])
                    centers.append((cX, cY))
                    self.logger.debug(f"Detected cone at ({cX}, {cY}) with area {area}.")
        return centers
