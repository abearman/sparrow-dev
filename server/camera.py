import auth
import os
import uuid

from flask import Blueprint, Flask
from flask import current_app, request, redirect, send_from_directory, url_for
from werkzeug import secure_filename

camera_api = Blueprint("camera_api", __name__)

UPLOAD_PATH = "photos/"
WIDE_ANGLE_CAMERA_PATH = "wide_angle/"

ALLOWED_EXTENSIONS = set(["png", "jpg", "jpeg"])

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS

@camera_api.route('/uploads/<filename>')
def uploaded_file(filename):
    app = current_app._get_current_object()
    print "here"
    return send_from_directory(app.config['UPLOAD_FOLDER'] + filename)

@camera_api.route("/camera/wide_angle/upload", methods=['GET', 'POST'])
@auth.requires_auth
def camera_wide_angle_upload():
    app = current_app._get_current_object()
    if request.method == "POST":
        file = request.files["file"]
        if file and allowed_file(file.filename):
            filename = secure_filename(file.filename)
            filepath = UPLOAD_PATH + WIDE_ANGLE_CAMERA_PATH
            print "Saving file to " + filepath
            file.save(os.path.join(app.config['UPLOAD_FOLDER'] + filepath,
                                   filename))
            return redirect(url_for('camera_api.uploaded_file',
                                    filename = filepath + filename))
    return '''
    <!doctype html>
    <title>Upload new File</title>
    <h1>Upload new File</h1>
    <form action="" method=post enctype=multipart/form-data>
      <p><input type=file name=file>
         <input type=submit value=Upload>
    </form>
    '''
