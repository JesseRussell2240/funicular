# image_processing.py

import cv2
import logging

class ImageProcessing:
    def __init__(self, config):
        """
        Initialize the ImageProcessing class with processing parameters and logging.
        
        Args:
            config (dict): Configuration dictionary containing processing parameters.
        """
        self.gaussian_kernel = config.get('gaussian_kernel', (5, 5))
        self.canny_threshold1 = config.get('canny_threshold1', 50)
        self.canny_threshold2 = config.get('canny_threshold2', 150)
        
        # Setup logger
        self.logger = logging.getLogger('ImageProcessing')
        self.logger.setLevel(logging.DEBUG)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)
        
    def preprocess(self, frame):
        """
        Apply preprocessing steps to the input frame.
        
        Args:
            frame (numpy.ndarray): The image frame in BGR format.
        
        Returns:
            numpy.ndarray: The preprocessed image.
        """
        self.logger.debug("Starting image preprocessing.")
        processed = frame.copy()
        
        # Apply Gaussian Blur to reduce noise
        processed = cv2.GaussianBlur(processed, self.gaussian_kernel, 0)
        self.logger.debug("Applied Gaussian Blur.")
        
        # Convert to grayscale
        gray = cv2.cvtColor(processed, cv2.COLOR_BGR2GRAY)
        self.logger.debug("Converted to grayscale.")
        
        # Apply Canny Edge Detection
        edges = cv2.Canny(gray, self.canny_threshold1, self.canny_threshold2)
        self.logger.debug("Applied Canny Edge Detection.")
        
        return edges
