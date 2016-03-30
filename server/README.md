# Sparrow Dev: Server

This README is meant to provide additional information on the server code contained in this folder.

Please see REQUIREMENTS.md for required dependencies to run server.

**auth.py** - Simple HTTP authentication; tango and server will likely be communicating over a private network connection,
so this code may not be necessary.

**camera.py** - HTTP endpoints for Tango to upload images to server for CV pipelines such as barcode recognition.

**camera_events.py** - Socket events for Tango to upload images; this file is essentially the socket version of camera.py.

**log_schema.py** - Schema for server event log; currently only records HTTP requests.

**pose.py** - HTTP endpoints for Tango to send pose data as well as endpoints for reading current pose data.

**pose_events.py** - Socket events for pose data uploads; this file is essentially the socket version of pose.py.

**server.py** - Server config and launch.

**tests** - in the process of adding some unit tests for endpoints; currently there are some more "manual" tests that open to
a webpage that will send a request to the server and then display outcome on screen.
