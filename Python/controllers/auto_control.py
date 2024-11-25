from flask import Blueprint

control_routes = Blueprint('control_routes', __name__)

autonomous_mode = False  # Flag for autonomous mode

@control_routes.route('/start_autonomous')
def start_autonomous():
    global autonomous_mode 
    autonomous_mode = True 
    return "Autonomous mode started"

@control_routes.route('/stop_autonomous')
def stop_autonomous():
    global autonomous_mode 
    autonomous_mode = False 
    move_stop()  # Ensure this function is defined   TBD####################
    return "Autonomous mode stopped"
