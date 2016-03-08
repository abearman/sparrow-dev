import auth
import os
import uuid

from flask import Blueprint, Flask, request

camera_api = Blueprint("camera_api", __name__)

@camera_api.route("/camera/wide_angle/upload", methods = ["POST"])
@auth.requires_auth
def camera_wide_angle_upload():
    if request.method == "POST":
        image = request.files["image"]
        extension = os.path.splitext(image.filename)[1]
        # Generate unique name for image file
        image_name = str(uuid.uuid4()) + extension
        # TODO (andrew): uploads folder/db?
        # app.config['UPLOAD_FOLDER'] = 'static/Uploads'
        # file.save(os.path.join(app.config['UPLOAD_FOLDER'], f_name))
        # return json.dumps({'filename':f_name})
