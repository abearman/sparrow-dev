# Dependencies
##### Please make sure the following python libraries are installed in order to run the server.
###### To install a library, run pip install <*library_name*> (you may need to run as sudo)
* eventlet==0.17.4
* Flask==0.10.1
* Flask-SocketIO==1.0
* Flask-WTF==0.9.5
* greenlet==0.4.2
* itsdangerous==0.24
* Jinja2==2.8
* MarkupSafe==0.23
* python-engineio==0.7.1
* python-socketio==0.6.1
* six==1.10.0
* Werkzeug==0.10.4
* WTForms==1.0.5
* flask-mongoengine

#####
install_dependencies.sh now tracks all depedencies, build here to get server running

##### At the moment, we are using MongoDB for the server's database.

###### Install MongoDB with Homebrew

```bash
brew install mongodb
mkdir -p /data/db
```
###### Set permissions for the data directory
Ensure that user account running mongod has correct permissions for the directory:

```bash
sudo chmod 0755 /data/db
sudo chown $USER /data/db
```

**Note:** If you get something like this:
```bash
exception in initAndListen: 10309 Unable to create/open lock file: /data/db/mongod.lock errno:13 Permission denied Is a mongod instance already running?, terminating
```

It means that `/data/db` lacks required permission and ownership.

Run `ls -ld /data/db/`

Output should look like this (`user_name` is directory owner and `wheel` is group to which user_name belongs):
```bash
drwxr-xr-x  7 user_name  wheel  238 Aug  5 11:07 /data/db/
