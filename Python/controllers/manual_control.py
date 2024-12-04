from flask import Blueprint, jsonify, request

# Create a Flask Blueprint for control routes
control_routes = Blueprint('control', __name__)

# State variables for Bluetooth and movement
bluetooth_enabled = False
movement_status = "stopped"

@control_routes.route('/control/move', methods=['POST'])
def move():
    
        return jsonify({"error": "Failed to process movement command."}), 500

@control_routes.route('/control/bluetooth', methods=['POST'])
def bluetooth():
    """
    Handle Bluetooth enable/disable actions.
    Expects JSON payload: {"action": "enable"} or {"action": "disable"}
    """
    global bluetooth_enabled
    try:
        # Parse the incoming JSON data
        action = request.json.get('action', 'disable')
        bluetooth_enabled = (action == 'enable')

        # Determine Bluetooth status
        status = "enabled" if bluetooth_enabled else "disabled"

        # Print the action to the console
        print(f"[Bluetooth] Status: {status}")

        # Respond with the current Bluetooth status
        return jsonify({"status": f"Bluetooth {status}"}), 200
    except Exception as e:
        print(f"[Error in Bluetooth Control] {e}")
        return jsonify({"error": "Failed to process Bluetooth command."}), 500
