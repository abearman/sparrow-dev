from server_state import app, socketio, default_host, default_port, HOST

import pose_events
import control_events
import camera_events

import mission_state

from control import drone_init, arm_and_takeoff

import optparse
import eventlet

# TODO: Add drone state
# { x, y, z, qx, qy, qz, qw, path }
# path should be saved to db

@app.route("/")
def home():
		return "Hello, from Sparrow!"

if __name__ == "__main__":

		parser = optparse.OptionParser()
		parser.add_option("-H", "--host",
											help="Hostname of the server (IP Address) " + \
													 "[default %s]" % default_host,
											default=default_host)
		parser.add_option("-P", "--port",
											help="Port for the server " + \
													 "[default %s]" % default_port,
											default=default_port)
		parser.add_option("-d", "--debug",
											action="store_true", dest="debug",
											help=optparse.SUPPRESS_HELP,
											default=False)

		parser.add_option("-s", "--static",
											help="Use static IP address defined in " + \
													 "server_state.py",
											action="store_true",
											dest="static_ip")

		options, _ = parser.parse_args()

		drone_init()

		print mission_state.vehicle

		server_options = {"binary": True}

		if (options.static_ip):
				socketio.run(app,
										 debug=options.debug,
										 host=HOST,
										 port=int(options.port))
										 
		else:
				socketio.run(app,
										 debug=options.debug,
										 host=options.host,
										 port=int(options.port))

		# socketio.run(app, debug=True)
		# DISABLE FOR PROD
		# app.debug = True
		# app.run()
		
		# ONLY ENABLE IF YOU WANT PUBLIC VISIBILITY
		# app.run(host="0.0.0.0") 
