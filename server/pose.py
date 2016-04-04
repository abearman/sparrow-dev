import auth

from mission_state import path, pose

from flask import Blueprint, Flask, request

pose_api = Blueprint("pose_api", __name__)

@pose_api.route("/pose/update", methods = ["POST"])
# @auth.requires_auth
def pose_update():
    global pose
    if request.method == "POST":
        pose = request.json
        print "[/pose/update]: pose is now " + str(pose)
    return ""

@pose_api.route("/pose/current")
# @auth.requires_auth
def pose_current():
    if pose == None:
        print "[/pose/current]: current pose is undefined"
        return "The tango is currently not initialized."
    else:
        print "[/pose/current]: current pose is " + str(pose)
        return "The tango's current pose is: " + str(pose)

@pose_api.route("/pose/path_config", methods = ["POST"])
def path_config():
    global path
    print request.get_data()
    print request.json
    if request.method == "POST":
        data = request.json['path']
        #print "[/pose/path_config]: received " + str(data["path"])
        path = []
        for coord_tuple in data:
            path.append( (coord_tuple['x'], coord_tuple['y'], coord_tuple['z']) )
        return "Sucess!"
        
