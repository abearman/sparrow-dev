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

HOST = "10.34.162.81" # This needs to be modified for server's IP    

socketio = SocketIO(app)
