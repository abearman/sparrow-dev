from flask import Flask, request
app = Flask(__name__)

pose = None # String representing tango location/rotation

@app.route("/")
def home():
    return "Hello, from Sparrow!"

@app.route("/pose/update", methods = ["POST"])
def pose_update():    
    global pose
    if request.method == "POST":
        data = request.form
        print "[/pose/update]: received "  + data.get("pose", None)
        pose = data["pose"]
        print "[/pose/update]: pose is now " + pose
    return ""

@app.route("/pose/current")
def pose_current():
    if pose == None:
        print "[/pose/current]: current pose is undefined"
        return "The tango is currently not initialized."
    else:
        print "[/pose/current]: current pose is " + pose
        return "The tango's current pose is: " + pose
    
if __name__ == "__main__":
    # DISABLE FOR PROD
    app.debug = True
    app.run()
    
    # ONLY ENABLE IF YOU WANT PUBLIC VISIBILITY
    # app.run(host="0.0.0.0") 
