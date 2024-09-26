# funicular
Selfdriving R-pi code


**Establishing SSH From Laptop To Rpi**
1. Get IP of RPI should be like 192.168.#.###

2. Enable SSH on the Rpi
   -Open Terminal on the Rpi and type **"sudo raspi-config"** in the configuration menu enable SSH
   -In CMD on laptop type "ssh rpi@rpi ip address found in step one" Ex. **"ssh rpi@192.168.#.###"**
   -If prompted with "WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED" it means you have an existing key under that IP address, type "ssh-keygen -R ip address found in step one" Ex. **"ssh-keygen -R 192.168.#.###"** This will clear that key
   -Now try **"ssh rpi@192.168.#.###"** again, when prompted to continue connected type **"yes"**
   -When prompted for the password, enter the Rpis password you set when configuring Ex **"rpi_password"**
   -SSH should now be established


**Camera Unit Testing instructions**
**this needs confirming**
1. pip install picamera opencv-python

2. run unit testing code - python test_camera_module.py
After this is running you should see a window with a live camera feed.Type 'q' to exit.


**Preception unit testing instructions**
**This needs confirming**
1. pip install opencv-python numpy

2. run unit testing code - python test_perception_module.py
preception also has a main to test preception and cameras joint integration. Ensure camera module is successfull prior to moving on to preception.
