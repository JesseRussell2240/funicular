from flask import Blueprint, jsonify
import json

control_routes = Blueprint('control_routes', __name__)

autonomous_mode = False  # Flag for autonomous mode

@control_routes.route('/start_autonomous')
def start_autonomous():
    global autonomous_mode
    autonomous_mode = True

    try:
        # Load JSON data when autonomous mode starts
        with open('/home/rpi/Desktop/Beta/Python/hailo-rpi5-examples/cone_detection.json', 'r') as json_file:  
            data = json.load(json_file)

        # Example: Log the data or perform an action with it
        print("Largest Red Cone:", data.get("largest_red_cone"))
        print("Largest Blue Cone:", data.get("largest_blue_cone"))
        
        # Optionally return the data as a response for testing
        return jsonify({
            "message": "Autonomous mode started, JSON data loaded",
            "data": data
        })

    except FileNotFoundError:
        return jsonify({"error": "JSON file not found"}), 404
    except json.JSONDecodeError:
        return jsonify({"error": "Invalid JSON format"}), 400

@control_routes.route('/stop_autonomous')
def stop_autonomous():
    global autonomous_mode
    autonomous_mode = False
    move_stop()  # Ensure this function is defined
    return "Autonomous mode stopped"
