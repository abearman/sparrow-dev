from server_state import app, socketio

import pose_events

# TODO: Add drone state
# { x, y, z, qx, qy, qz, qw, path }
# path should be saved to db

@app.route("/")
def home():
    return "Hello, from Sparrow!"

if __name__ == "__main__":
    # socketio.run(app, host=HOST, debug=True)
    socketio.run(app, debug=True)
    # DISABLE FOR PROD
    # app.debug = True
    # app.run()
    
    # ONLY ENABLE IF YOU WANT PUBLIC VISIBILITY
    # app.run(host="0.0.0.0") 
