import json

# path = None; # List of path coordinates (x, y, z)
path = {'path': [{'x': 0.0, 'y': 0.0, 'z': 0.0}, {'x': 5.0, 'y': 0.0, 'z': 0.0}]}

pose = None; # JSON object representing Tango measured location/rotation

vehicle = None;
airspeed = 3;

frame_counter = 0;
gps_init = False;

origin_lla = (37.430032, -122.173439, 20)
origin_ecef = (37.430032, -122.173439, 20)
