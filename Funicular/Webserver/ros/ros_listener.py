# ros_listener.py
#not tested
import rospy
from std_msgs.msg import Float32
from flask_socketio import SocketIO

# Assuming that Flask-SocketIO is used for real-time communication
socketio = None  # Will be set from app.py

def ros_init():
    """ Initialize ROS node and start subscribing to necessary topics """
    rospy.init_node('flask_ros_listener', anonymous=True)

    # Subscribe to ROS topics (e.g., for speed, acceleration, and g-force)
    rospy.Subscriber('/car/speed', Float32, speed_callback)
    rospy.Subscriber('/car/acceleration', Float32, acceleration_callback)
    rospy.Subscriber('/car/gforce', Float32, gforce_callback)

    # Keep the node running
    rospy.spin()

def speed_callback(data):
    """ Handle incoming speed data from ROS and send to the client """
    print(f"Speed: {data.data}")
    if socketio:
        socketio.emit('speed_update', {'speed': data.data})

def acceleration_callback(data):
    """ Handle incoming acceleration data from ROS and send to the client """
    print(f"Acceleration: {data.data}")
    if socketio:
        socketio.emit('acceleration_update', {'acceleration': data.data})

def gforce_callback(data):
    """ Handle incoming g-force data from ROS and send to the client """
    print(f"G-Force: {data.data}")
    if socketio:
        socketio.emit('gforce_update', {'gforce': data.data})
