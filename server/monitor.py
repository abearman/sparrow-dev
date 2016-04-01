import auth

from flask import Blueprint, Flask, request, render_template

monitor_api = Blueprint("monitor_api", __name__)

@monitor_api.route("/monitor/map")
def map():
    print "[/monitor/map]: returning drone_monitor.html"
    return render_template('drone_monitor.html')
