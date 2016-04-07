0.6.6
-----
- Added SocketIO.off() and SocketIO.once()

0.6.5
-----
- Updated wait loop to be more responsive under websocket transport

0.6.4
-----
- Fixed support for Python 3
- Fixed thread cleanup

0.6.3
-----
- Upgraded to socket.io protocol 1.x for websocket transport
- Added locks to fix concurrency issues with polling transport
- Fixed SSL support

0.6.1
-----
- Upgraded to socket.io protocol 1.x thanks to Sean Arietta and Joe Palmer

0.5.6
-----
- Backported to support requests 0.8.2

0.5.5
-----
- Fixed reconnection in the event of server restart
- Fixed calling on_reconnect() so that it is actually called
- Set default Namespace=None
- Added support for Python 3.4

0.5.3
-----
- Updated wait loop to exit if the client wants to disconnect
- Fixed calling on_connect() so that it is called only once
- Set heartbeat_interval to be half of the heartbeat_timeout

0.5.2
-----
- Replaced secure=True with host='https://example.com'
- Fixed sending heartbeats thanks to Travis Odom

0.5.1
-----
- Added error handling in the event of websocket timeout
- Fixed sending acknowledgments in custom namespaces thanks to Travis Odom

0.5
---
- Rewrote library to use coroutines instead of threads to save memory
- Improved connection resilience
- Added support for xhr-polling thanks to Francis Bull
- Added support for jsonp-polling thanks to Bernard Pratz
- Added support for query params and cookies

0.4
---
- Added support for custom headers and proxies thanks to Rui and Sajal
- Added support for server-side callbacks thanks to Zac Lee
- Added low-level _SocketIO to remove cyclic references
- Merged Channel functionality into BaseNamespace thanks to Alexandre Bourget

0.3
---
- Added support for secure connections
- Added SocketIO.wait()
- Improved exception handling in _RhythmicThread and _ListenerThread

0.2
---
- Added support for callbacks and channels thanks to Paul Kienzle
- Incorporated suggestions from Josh VanderLinden and Ian Fitzpatrick

0.1
---
- Wrapped `code from StackOverflow <http://stackoverflow.com/questions/6692908/formatting-messages-to-send-to-socket-io-node-js-server-from-python-client>`_
- Added exception handling to destructor in case of connection failure
