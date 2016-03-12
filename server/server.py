import auth

from flask import Flask, request
from flask_socketio import SocketIO

from camera import camera_api
from pose import pose_api

app = Flask(__name__)

app.register_blueprint(pose_api)

socketio = SocketIO(app)

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
