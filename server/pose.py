import auth

from flask import Blueprint, Flask, request

pose_api = Blueprint("pose_api", __name__)

pose = None # String representing tango location/rotation

@pose_api.route("/pose/update", methods = ["POST"])
@auth.requires_auth
def pose_update():
    global pose
    if request.method == "POST":
        data = request.json
        print "[/pose/update]: received "  + data["pose"]
        pose = data["pose"]
        print "[/pose/update]: pose is now " + pose
    return ""

@pose_api.route("/pose/current")
@auth.requires_auth
def pose_current():
    if pose == None:
        print "[/pose/current]: current pose is undefined"
        return "The tango is currently not initialized."
    else:
        print "[/pose/current]: current pose is " + pose
        return "The tango's current pose is: " + pose
