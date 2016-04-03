import auth

from flask import Flask, request
from flask.ext.mongoengine import MongoEngine
from flask_socketio import SocketIO

from camera import camera_api
from pose import pose_api
from monitor import monitor_api

app = Flask(__name__)
app.config["MONGODB_SETTINGS"] = {'DB': "sparrow_server_event_log"}

db = MongoEngine(app)

app.register_blueprint(pose_api)
app.register_blueprint(monitor_api)

socketio = SocketIO(app)

# TODO: Add drone state
# { x, y, z, qx, qy, qz, qw, path }
# path should be saved to db

@app.route("/")
def home():
    return "Hello, from Sparrow!"
    
if __name__ == "__main__":
    socketio.run(app, debug = True)
    # DISABLE FOR PROD
    # app.debug = True
    # app.run()
    
    # ONLY ENABLE IF YOU WANT PUBLIC VISIBILITY
    # app.run(host="0.0.0.0") 
