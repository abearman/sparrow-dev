import auth

from flask import Flask, request

from camera import camera_api
from pose import pose_api

app = Flask(__name__)

app.register_blueprint(pose_api)

@app.route("/")
def home():
    return "Hello, from Sparrow!"
    
if __name__ == "__main__":
    # DISABLE FOR PROD
    app.debug = True
    app.run()
    
    # ONLY ENABLE IF YOU WANT PUBLIC VISIBILITY
    # app.run(host="0.0.0.0") 
