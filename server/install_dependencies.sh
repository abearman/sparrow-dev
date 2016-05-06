#!/bin/bash
dep_ubuntu="eventlet==0.17.4 Flask==0.10.1 Flask-SocketIO==1.0 Flask-WTF==0.9.5 greenlet==0.4.2 
						itsdangerous==0.24 Jinja2==2.8 MarkupSafe==0.23 python-engineio==0.7.1 python-socketio==0.6.1 six==1.10.0
						Werkzeug==0.10.4 WTForms==1.0.5 flask-mongoengine Flask-JSGlue Pillow"

length=$(echo $dep_ubuntu | wc -w)

for pkg in $dep_ubuntu; do
    echo "Processing ${pkg}, Package $pkg of $length"
    sudo pip install ${pkg}
done

sudo python socketIO-client/setup.py install
